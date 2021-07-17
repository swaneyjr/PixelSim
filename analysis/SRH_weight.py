#!/usr/bin/env python

import itertools as it

import numpy as np
import matplotlib.pyplot as plt
import ROOT as r

from scipy.interpolate import interp1d
from scipy.stats import binom, lognorm
from scipy.signal import convolve, savgol_filter
from scipy.optimize import minimize, Bounds, LinearConstraint

from efficiency import sim_eff

def binom_conv(n_electrons, eff, p, fast=False):
    if fast: 
        return interp1d(n_electrons, eff, kind='cubic', 
                bounds_error=False, fill_value=0)(n_electrons/p)

    pdf = -np.diff(eff)
    
    pdf_conv = np.zeros(pdf.size)
    nnz = n_electrons[1:] > 0
    for n, freq in zip(n_electrons[1:][nnz], pdf[nnz]):
        n_conv = binom.pmf(n_electrons[1:], n, p)
        pdf_conv += freq * n_conv
    return np.hstack([np.cumsum(pdf_conv[::-1])[::-1], [0]])
 
def gain_conv(n_electrons, eff, sigma):
    if not sigma: return eff

    pdf = -np.diff(eff)

    conv = lognorm(sigma)
    pdf_conv = np.zeros(pdf.size)
    nnz = n_electrons[1:] >= 0.1
    for n, freq in zip(n_electrons[1:][nnz], pdf[nnz]):
        n_conv = conv.pdf(n_electrons[1:]/n)
        pdf_conv += freq * n_conv / n_conv.sum()

    return np.hstack([np.cumsum(pdf_conv[::-1])[::-1], [0]])


# use an optimizer 
def find_weighting(funcs, goal, w=None, norm=False, expansion=None):

    funcs = np.array(funcs)
    if w is None:
        w = np.ones(goal.size)

    if expansion:
        return _weighting_expansion(funcs, goal, w, norm, expansion)

    else:
        return _weighting_hist(funcs, goal, w, norm)


def _weighting_hist(funcs, goal, w, norm):
 
    n = len(funcs)

    # compute constant Hessian
    _hess = 2*np.sum(w * funcs.reshape(1,n,-1) * funcs.reshape(n,1,-1), axis=2)

    def _obj_func(coeffs, funcs_, goal_, w_):
        fit = np.sum(coeffs.reshape(-1,1)*funcs_, axis=0)
        return np.sum(w_*(goal_ - fit)**2, axis=0)

    def _obj_jac(coeffs, funcs_, goal_, w_):
        fit = np.sum(coeffs.reshape(-1,1)*funcs_, axis=0)
        return -2*np.sum(w_*(goal_-fit)*funcs_, axis=1)

    def _obj_hess(coeffs, funcs_, goal_, w_):
        return _hess

    x0 = np.ones(n) / n
    bounds = Bounds(0, np.inf)

    if norm:
        constraints = [LinearConstraint(np.ones((1,n)), [1], [1])]
    else:
        constraints = []

    res = minimize(_obj_func, x0, args=(funcs, goal, w), method='trust-constr',
            jac=_obj_jac, hess=_obj_hess,
            constraints=constraints, bounds=bounds)

    return res.x, res.fun

# instead of finding each bin coefficient separately,
# use a function a_n = exp(b0 + b1*n + b1*n^2 + ...)
# a_i: coeffs
# b_i: params
def _weighting_expansion(funcs, goal, w, norm, order):

    n = len(funcs)
    x = np.linspace(-1, 1, n)

    # compute constant Hessian
    _hess_coeffs = 2*np.sum(w * funcs.reshape(1,n,-1) * funcs.reshape(n,1,-1), axis=2)

    def _get_coeffs(params):
        expansion = params*x.reshape(-1,1)**np.arange(order)
        return np.exp(expansion.sum(axis=1))

    def _obj_func(params, funcs_, goal_, w_):
        coeffs = _get_coeffs(params)
        fit = np.sum(coeffs.reshape(-1,1)*funcs_, axis=0)
        return np.sum(w_*(goal_ - fit)**2, axis=0)

    def _obj_jac(params, funcs_, goal_, w_):
        coeffs = _get_coeffs(params)
        fit = np.sum(coeffs.reshape(-1,1)*funcs_, axis=0)
        jac_coeffs = -2*np.sum(w_*(goal_-fit)*funcs_, axis=1)
        jac_params = coeffs.reshape(-1,1) * x.reshape(-1,1)**np.arange(order)
        return jac_coeffs.dot(jac_params)

    def _obj_hess(params, funcs_, goal_, w_):
        coeffs = _get_coeffs(params)
        fit = np.sum(coeffs.reshape(-1,1)*funcs_, axis=0)
        jac_coeffs = -2*np.sum(w_*(goal_-fit)*funcs_, axis=1)
        jac_params = coeffs.reshape(-1,1) * x.reshape(-1,1)**np.arange(order)
        hess_params = coeffs.reshape(-1,1,1)*x.reshape(-1,1,1)**(np.arange(order).reshape(-1,1) + np.arange(order))

        return jac_params.T.dot(_hess_coeffs).dot(jac_params) \
                + np.sum(jac_coeffs.reshape(-1,1,1)*hess_params, axis=0)

    x0 = np.zeros(order)

    if norm:
        raise NotImplementedError

    res = minimize(_obj_func, x0, args=(funcs, goal, w), method='BFGS',
            jac=_obj_jac)

    coeffs = _get_coeffs(res.x)
    return coeffs, res.fun


def average_targets(f_targets, xvals, smooth=True):
    xcut = np.ones(xvals.size, dtype=bool)
    eff_x = []
    err_x = []
    for fname in f_targets:
        f = np.load(fname)

        cut = f.f.eff > 0

        x = f.f.electrons[cut]
        eff = savgol_filter(f.f.eff[cut], 21, 3) if smooth else f.f.eff[cut]
        err = savgol_filter(f.f.err[cut], 21, 3) if smooth else f.f.err[cut]

        xcut &= (xvals >= x.min()) & (xvals <= x.max())
        eff_x.append(interp1d(x, eff, 
            kind='cubic', fill_value='extrapolate')(xvals))
        err_x.append(interp1d(x, err, 
            kind='cubic', fill_value='extrapolate')(xvals))

        f.close()
    
    eff_all = np.vstack(eff_x).T[xcut]
    err_all = np.vstack(err_x).T[xcut]
    w_all = err_all**-2

    eff = np.average(eff_all, axis=1, weights=w_all)
    err = 1 / np.sqrt(np.sum(w_all, axis=1))

    return eff, err, xcut

def apply_weights(n_electrons, eff, p, coeff, gain_err=0, fast=False):
    eff_tot = np.zeros(n_electrons.size)

    for pi, ci in zip(p, coeff):
        if pi < 1e-5: continue
        eff_p = binom_conv(n_electrons, eff, pi, fast=fast)
        eff_tot += ci * eff_p

    if gain_err:
        return gain_conv(n_electrons, eff_tot, gain_err)
    else:
        return eff_tot

if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser()

    parser.add_argument('--sim', required=True, help='Simulation .root file')
    parser.add_argument('--acc_ratio', type=float, default=1, help='Acceptance factor for simulated data')
    parser.add_argument('--gain_err', type=float, default=0, help='Spread of gains for individual pixels')
    parser.add_argument('--target', required=True, nargs='+', help='.npz data files to fit')
    parser.add_argument('--xlim', nargs=2, default=[1,1000], type=float, help='Minimum and maximum threshold bins')
    parser.add_argument('--plim', nargs=2, default=[.1, 1], type=float, help='Minimum and maximum probability bins')
    parser.add_argument('--np', type=int, default=9, help='Number of probability bins')
    parser.add_argument('--nx', type=int, default=25, help='Number of bins on threshold axis')
    parser.add_argument('--expansion', type=int, default=0, help='Order of polynomial expansion model, or 0 for a bin fit')
    parser.add_argument('-p', '--logp', action='store_true', help='Generate p bins on a logarithmic scale')
    parser.add_argument('-x', '--logx', action='store_true', help='Generate x bins on a logarithmic scale')
    parser.add_argument('-n', '--norm', action='store_true', help='Normalize bins')
    parser.add_argument('-f', '--fast', action='store_true', help='Only scale distributions by a factor of p')
    parser.add_argument('-P', '--plot', action='store_true', help='Plot results')
    parser.add_argument('--out', help='Name of result file (.npz extension)')

    args = parser.parse_args()

    print('Loading simulated data...', end='', flush=True)
    f_sim = r.TFile(args.sim)
    n_electrons, eff = sim_eff(f_sim)
    f_sim.Close()
    print('Done')

    eff *= args.acc_ratio

    if args.plot:
        for fname in args.target:
            f = np.load(fname)
            plt.plot(f.f.electrons, f.f.eff)
            plt.fill_between(f.f.electrons, f.f.eff-f.f.err, f.f.eff+f.f.err, alpha=0.2)
            f.close()
    
    if args.logp:
        p1, p0 = args.plim
        pvals = 1 - np.geomspace(1-p0, 1-p1, args.np) 
    else:
        pvals = np.linspace(*args.plim, args.np)

    xvals = np.geomspace(*args.xlim, args.nx) if args.logx \
            else np.linspace(*args.xlim, args.nx)

    print('Creating goal function...', end='', flush=True)
    goal, goal_err, xcut = average_targets(args.target, xvals)
    xvals = xvals[xcut] 

    if args.plot:
        plt.errorbar(xvals, goal, yerr=goal_err, c='gold', ls='', marker='+')
    
    print('Done')
    print('Convolving simulation efficiencies...', end='', flush=True)
    funcs = []
    plotfuncs = []
    for i,p in enumerate(pvals):

        printstr = '{}/{}'.format(i+1, len(pvals))
        print(printstr, end='\b'*len(printstr), flush=True)
        
        # apply binomial
        func_p = binom_conv(n_electrons, eff, p, fast=args.fast)

        if args.gain_err:
            func_p = gain_conv(n_electrons, func_p, args.gain_err)
        
        plotfuncs.append(func_p)

        # set a more reasonable cutoff
        cut = func_p > 1e-10
        n = n_electrons[cut]
        func_p = func_p[cut]

        # plot before doing linear interpolation
        #if args.plot:    
        #    plt.plot(n, func_p, label=p)
        
        func_p = interp1d(n, func_p, kind='cubic')(xvals)
        funcs.append(func_p)

    print('Done', ' ' * (len(printstr)-4))

    print('Calculating weights...', end='', flush=True)
    coeffs, loss = find_weighting(funcs, goal, goal_err**-2, 
            norm=args.norm, expansion=args.expansion)
    print('Done    ')
    print('Loss = {:.3f}'.format(loss / goal.size))
    print()

    for p, coeff in zip(pvals, coeffs):
        if coeff > 1e-5:
            print('{:.2f}: {}'.format(p, coeff))

    if args.plot:
        #yvals = apply_weights(n_electrons, eff, pvals, coeffs, args.gain_err)
        yvals = np.sum([ai * fi for ai, fi in zip(coeffs, plotfuncs)], axis=0)
        plt.plot(n_electrons, yvals, label='Weighted')

        if args.logx: plt.semilogx()
        
        plt.semilogy()
        plt.legend()
        plt.show()

    if args.out:
        np.savez(args.out,
                p=pvals,
                x=xvals,
                gain_err=args.gain_err,
                coeffs=coeffs/coeffs.sum(),
                fast=args.fast)


