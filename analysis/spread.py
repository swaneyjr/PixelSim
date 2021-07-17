#!/usr/bin/env python3

import ROOT as r
import numpy as np
import matplotlib.pyplot as plt

from efficiency import get_total_events

COLZ = ['royalblue', 'crimson', 'gold']
STYLZ = ['full', 'full']#['solid', 'dotted', 'dashed']

STATS = {
        'sum5': (5,1),
        'sum9': (9,1.5),
        'sum21': (21,2.5)
        }

KERNELS = {
        'sum5': np.array([[0,0,0,0,0],
                          [0,0,1,0,0],
                          [0,1,1,1,0],
                          [0,0,1,0,0],
                          [0,0,0,0,0]]),

        'sum9': np.array([[0,0,0,0,0],
                          [0,1,1,1,0],
                          [0,1,1,1,0],
                          [0,1,1,1,0],
                          [0,0,0,0,0]]),

        'sum21':np.array([[0,1,1,1,0],
                          [1,1,1,1,1],
                          [1,1,1,1,1],
                          [1,1,1,1,1],
                          [0,1,1,1,0]]),
        }

def get_total_frames(f):
    trig = f.Get('triggers')
    untrig = f.Get('nontriggers')
    return trig.GetEntries() + untrig.GetEntries()

def get_sim(f, stat, noise_gen, thresh, p_mask):
    t = f.Get('hits')

    n, rmax = STATS[stat]

    edep_max = []
    edep_adj = []

    for evt in t:
        pix_n = np.array(list(evt.n_tot)) - thresh
        pix_n += noise_gen(pix_n.size)
        pix_n = np.maximum(pix_n, 0)

        if len(evt.n_tot) == 1:
            pix_n_other = noise_gen(n-1)
            pix_n_other = np.maximum(pix_n_other, 0)
            pix_n_other[np.random.rand(n-1) < p_mask] = 0

            edep_max.append(pix_n[0])
            edep_adj.append(pix_n[0] + pix_n_other.sum())


        elif len(evt.n_tot) > 1:
            pix_xy = np.vstack([list(evt.pix_x), list(evt.pix_y)]).T

            # find max pixel and any adjacent hits
            idx_max = np.argmax(pix_n)
            pix_dxy = pix_xy - pix_xy[idx_max]
            adj = np.sum(pix_dxy**2, axis=1) <= rmax**2
            pix_n[adj][np.random.rand(adj.sum()) < p_mask] = 0

            pix_n_other = noise_gen(n-adj.sum())
            pix_n_other = np.maximum(pix_n_other, 0)
            pix_n_other[np.random.rand(n-adj.sum()) < p_mask] = 0

            edep_max.append(pix_n[idx_max])
            edep_adj.append(pix_n[adj].sum() + pix_n_other.sum())

    return np.array(edep_max), np.array(edep_adj)


def get_data(f, stat):
    edep_max = []
    edep_adj = []

    triggers = f.Get('triggers')
    for trig in triggers:
        
        # make sure this isn't biased by saturation
        if max(trig.raw) == 1023: continue

        # check for hot pixels which could skew the result
        #raw = np.array(list(trig.raw))
        #cal = np.array(list(trig.cal))
        
        #hot = ((raw > 0) & (cal == 0)).reshape(5,5)
        #n_hot = np.sum(hot * KERNELS[stat])

        #print(n_hot)
        #if n_hot: continue

        edep_max.append(trig.max)
        edep_adj.append(getattr(trig, stat))

    return np.array(edep_max), np.array(edep_adj)

    
def plot_hist(ax, edep_max, edep_adj, n_tot=None, 
        bg_max=None, bg_adj=None, bg_tot=None, label='', 
        bins=100, ranges=[], **kwargs):
    if not ranges:
        ranges = [(0, np.inf)]
    for i, rng in enumerate(ranges):
        emin, emax = rng
        cut = (edep_max >= emin) & (edep_max < emax)
        r = edep_adj[cut]/edep_max[cut] - 1
        r_hist, bins = np.histogram(r, bins=bins)

        if bg_tot:
            cut_bg = (bg_max >= emin) & (bg_max < emax)
            rbg = bg_adj[cut_bg] / bg_max[cut_bg] - 1

            bg_hist, bins = np.histogram(rbg, bins=bins)

            corr_hist = r_hist / n_tot - bg_hist / bg_tot
            corr_err = np.sqrt(r_hist / n_tot**2 + bg_hist/bg_tot**2)

        else:
            corr_hist = r_hist
            corr_err = np.sqrt(r_hist)
 
        norm = corr_hist.sum()
        if len(ranges) == 1:
            color = 'k'
            lbl = label
        else:
            color = COLZ[i]
            lbl = label + ': [{},{})'.format(int(emin), int(emax))
        ax.errorbar(bins[:-1], corr_hist/norm, yerr=corr_err/norm, 
                color=color, mec=color, marker='.', ls='', label=lbl, **kwargs)
        
        yield (corr_hist/norm, corr_err/norm)

if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Calculate deposited energy and efficiency curves')
    parser.add_argument('--sim', nargs='+', default=[], help='.root file with simulation data')
    parser.add_argument('--sim_label', nargs='+', default=[], help='Labels for simulation .root files')
    parser.add_argument('--data', nargs='+', default=[], help='.root file(s) with experimental data, to be plotted separately')
    parser.add_argument('--data_label', nargs='+', default=[], help='Labels for data .npz files')
    parser.add_argument('--bg', nargs='+', default=[], help='Background runs corresponding to --data')
    parser.add_argument('--ranges', nargs='+', type=float, default=[])
    parser.add_argument('--ylim', nargs=2, type=float)
    parser.add_argument('--title')
    parser.add_argument('--gain', nargs='+', type=float, default=[])
    parser.add_argument('--stat', choices=STATS.keys(), default='sum5')
    parser.add_argument('--noise', type=float, default=0, help='Poisson noise to add to simulation')
    parser.add_argument('--thresh', type=int, default=0, help='Effective black level in electrons to add to simulated data')
    parser.add_argument('--noisefile', nargs='+', default=[], help='histogram .npz files to use for noise statistics')
    parser.add_argument('-x', '--logx', action='store_true')
    parser.add_argument('-y', '--logy', action='store_true')
    parser.add_argument('--nbins', type=int)
    parser.add_argument('--pmask', type=float, default=0, help='Probability to mask a pixel as hot')
    parser.add_argument('--small', action='store_true', help='generate small figure')

    args = parser.parse_args()

    figsize = (4,2.5) if args.small else (5,4)
    plt.figure(figsize=figsize, tight_layout=True)

    plt.title(args.title)
    plt.xlabel('$N_{' + str(STATS[args.stat][0]-1) + '}\,/\,N_\mathrm{max}$')
    plt.ylabel('Probability')
    if args.logy:
        plt.semilogy()
    if args.logx:
        plt.semilogx()
        bins = np.geomspace(.002, 4, args.nbins)
    else:
        bins = np.linspace(0, 4, args.nbins)

    ax = plt.gca()

    ranges = []
    if args.ranges:
        for i in range(len(args.ranges)-1):
            ranges.append((args.ranges[i], args.ranges[i+1]))
    
    for i,fname in enumerate(args.sim): 

        if i < len(args.noisefile):
            fn = np.load(args.noisefile[i])
            bins_noise = np.arange(fn.f.counts.size+1).astype(float)
            prob = fn.f.counts / fn.f.counts.sum()
            cdf = np.cumsum(np.hstack([[0], prob]))
            g = args.gain[i] if i < len(args.gain) else 1
            noise_gen = lambda n: (np.interp(np.random.rand(n), cdf, bins_noise)/g).astype(int)
            fn.close()
        else:
            noise_gen = lambda n: np.random.poisson(args.noise, n)

        f = r.TFile(fname)
        n_max, n_adj = get_sim(f, args.stat, noise_gen, args.thresh, args.pmask)
        
        f.Close()

        label = args.sim_label[i] if i < len(args.sim_label) \
                else fname.split('/')[-1][:-5].replace('_', ' ')
        eff_sim, err_sim = list(plot_hist(ax, n_max, n_adj, bins=bins, 
                ranges=ranges, label=label, mfc='white'))[0]

    for i,fname in enumerate(args.data):
                
        f = r.TFile(fname)
        n_max, n_adj = get_data(f, args.stat)
        n_evt = get_total_frames(f)
        f.Close()
        
        if i < len(args.bg):
            fbg = r.TFile(args.bg[i])
            bg_max, bg_adj = get_data(fbg, args.stat)
            bg_evt = get_total_frames(fbg)
            fbg.Close()
        else:
            bg_max = np.array([])
            bg_adj = np.array([])
            bg_evt = 0

        label = args.data_label[i] if i < len(args.data_label) else 'Data'
        gain = args.gain[i] if i < len(args.gain) else 1
        eff_data, err_data = list(plot_hist(ax, n_max/gain, n_adj/gain, n_evt, 
                bg_max/gain, bg_adj/gain, bg_evt,
                label=label, bins=bins, ranges=ranges))[0]

        print(np.nansum((eff_data - eff_sim)**2/(err_data**2 + err_sim**2)))


    ax.xaxis.set_label_coords(.90, -0.2)
    ax.legend(loc='upper right')

    if args.ylim:
        ax.set_ylim(*args.ylim)
    plt.show()

    
