#!/usr/bin/env python3

import ROOT as r
import numpy as np
import matplotlib.pyplot as plt

SIM_COLZ = ['k', 'dimgray', 'darkgray', 'gray']
DATA_COLZ = ['royalblue', 'crimson', 'gold']

def get_total_events(f):
    n_evt = 0

    f.cd('primaries')
    for k in r.gDirectory.GetListOfKeys():
        h = f.Get('primaries/' + k.GetName())
        n_evt += h.GetEntries()

    return n_evt

def sim_eff(f):
    n_evt_tot = get_total_events(f)
    t = f.Get('hits')

    edep_max = []

    n_evt = t.GetEntries()
    for evt in t:
        if len(evt.n_tot):
            edep_max.append(max(evt.n_tot))
        else:
            edep_max.append(0)
    
    max_bins = np.arange(max(edep_max)+2)
    max_hist = np.histogram(edep_max, bins=max_bins)[0]

    # add any events skipped by "minPix" filter
    max_hist[0] += n_evt_tot - n_evt

    return max_bins[:-1], 1 - np.cumsum(max_hist/max_hist.sum())


def plot_sim(ax, f, label, f_weighting=None, acc_ratio=1, scale=None, scale_label=None, color='k'):
    
    electrons, eff = sim_eff(f)
    
    if f_weighting:
        from SRH_weight import apply_weights
        weight = np.load(f_weighting)
        eff = apply_weights(electrons, eff, weight.f.p, weight.f.coeffs, 
                weight.f.gain_err, fast=weight.f.fast)
        weight.close()

    eff *= acc_ratio

    # val >= thresh
    # also don't show zero bin
    ax.plot(electrons[1:-1], eff[1:-1], c=color, ls='-', label=label)

    if scale:
        ax.plot(electrons[1:-1], eff[1:-1]*scale, c=color, ls='--', label=scale_label)

def plot_data(ax, data, label, c=None):
    n_e = data['electrons']
    eff = data['eff']
    err = data['err']

    print(label)
    #hist = -np.diff(np.hstack([eff_max, 0]))
    #err_hist = np.sqrt(err[:-1]**2 + err[1:]**2)

    xmin, xmax = ax.get_xlim()
    ax.plot(n_e, eff, label=label, c=c)
    ax.fill_between(n_e, eff-err, eff+err, color=c, alpha=0.2, edgecolor=None)
    ax.set_xlim(max(1, min(xmin, n_e.min()/1.5)), max(xmax, n_e.max()*1.5))


if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Calculate deposited energy and efficiency curves')
    parser.add_argument('rootfiles', nargs='+', help='.root file with simulation data')
    parser.add_argument('--sim_label', nargs='+', default=[], help='Labels for simulation .root files')
    parser.add_argument('--acc_ratio', type=float, default=[], nargs='+', help='Multiplier for efficiency values')
    parser.add_argument('--data_corr', type=float, help='Presumed relative offset due to uncertainty in activity')
    parser.add_argument('--scale_all', type=float, help='Add second axis with scaled values')
    parser.add_argument('--scale_label', default='Scaled', help='Label for scaled axis')
    parser.add_argument('--weighting', help='Weighting .npz file from SRH_weight.py')
    parser.add_argument('--data', nargs='+', default=[], help='.root file(s) with experimental data, to be plotted separately')
    parser.add_argument('--data_label', nargs='+', default=[], help='Labels for data .npz files')
    parser.add_argument('--xlim', nargs=2, type=float)
    parser.add_argument('--ylim', nargs=2, type=float)
    parser.add_argument('-A', '--use_acceptance', action='store_true', help='y label is total acceptance A epsilon')
    parser.add_argument('--title')
    parser.add_argument('--small', action='store_true', help='Generate small plot')

    args = parser.parse_args()

    figsize = (3.2, 3.2) if args.small else (4, 4)
    plt.figure(figsize=figsize, tight_layout=True)

    ax = plt.gca()

    # start with a minimal range
    if args.data:
        ax.set_xlim(100,101)

    for i,f in enumerate(args.data):
        label = args.data_label[i] if i < len(args.data_label) else None
        data = np.load(f)
        plot_data(ax, data, label, c=DATA_COLZ[i])

    for i,fname in enumerate(args.rootfiles):
        label = args.sim_label[i] if i < len(args.sim_label) \
                else fname.split('/')[-1][:-5].replace('_', ' ')

        acc = args.acc_ratio[i] if i < len(args.acc_ratio) else 1

        f = r.TFile(fname)        
        color = DATA_COLZ[i] if not args.data else SIM_COLZ[i]
        
        scale = args.scale_all
        scale_label = label + ' ({})'.format(args.scale_label)
        if args.data_corr:
            scale = 1 + args.data_corr
            scale_label = '{:+.1f}% adjustment'.format(100*corr)

        plot_sim(ax, f, label, args.weighting, acc, scale=scale, 
                scale_label=scale_label, color=color)

        f.Close() 


    #ax.xaxis.set_label_coords(.95, -0.1) 

    ax.loglog()
    ax.legend()

    ax.set_title(args.title)
    ax.set_xlabel('Threshold [$e^-$]')
    if args.use_acceptance:
        ax.set_ylabel(r'$A\epsilon$ [$\mathrm{m}^2$]')
    else:
        ax.set_ylabel('Efficiency')

    if args.xlim:
        ax.set_xlim(*args.xlim)
    if args.ylim:
        ax.set_ylim(*args.ylim)

    #if args.scale_all:
    #    ax_scale = ax.twinx()
    #    ax_scale.set_ylabel(args.scale_label)
    #    scale_lim = args.scale_all * np.array(ax.get_ylim())
    #    ax_scale.set_ylim(*scale_lim)
    #    ax_scale.loglog()

    plt.show()

    
