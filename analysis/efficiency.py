#!/usr/bin/env python3

import ROOT as r
import numpy as np
import matplotlib.pyplot as plt

def make_plots(tdict, gain=1, white=None):

    if white is None: white = np.inf

    xunit = r'$N_e$' if gain==1 else 'ADC'
    
    plt.figure(figsize=(16, 12))
    plt.tight_layout()

    ax1 = plt.subplot(221)
    plt.title('Max pixel')
    plt.xlabel(xunit)
    plt.ylabel('Freq')

    ax2 = plt.subplot(222)
    plt.title('Total')
    plt.xlabel(xunit)
    plt.ylabel('Freq')

    ax3 = plt.subplot(223)
    plt.title('Max pixel')
    plt.xlabel(xunit + ' thresh')
    plt.ylabel('Eff')

    ax4 = plt.subplot(224)
    plt.title('Total')
    plt.xlabel(xunit + ' thresh')
    plt.ylabel('Eff') 

    for fname, t in tdict.items():

        label = fname[:-5].replace('_', ' ')

        edep_max = []
        edep_tot = []

        for evt in t:
            if len(evt.pix_e):
                gain_adj = [min(int(gain*e), white) for e in evt.pix_e]
                edep_max.append(max(gain_adj))
                edep_tot.append(sum(gain_adj))
            else:
                edep_max.append(0)
                edep_tot.append(0)
        
        max_bins = np.arange(min(white, max(edep_max))+1)
        tot_bins = np.arange(max(edep_tot)+1)

        max_hist = np.histogram(edep_max, bins=max_bins)[0]
        tot_hist = np.histogram(edep_tot, bins=tot_bins)[0]

        max_hist = max_hist / max_hist.sum()
        tot_hist = tot_hist / tot_hist.sum()

        max_eff = 1 - np.cumsum(max_hist)
        tot_eff = 1 - np.cumsum(tot_hist)

        # do not display zero bin
        max_bins = max_bins[1:-1]
        tot_bins = tot_bins[1:-1]

        ax1.plot(max_bins, max_hist[1:], label=label)
        ax2.plot(tot_bins, tot_hist[1:], label=label)
        # val != thresh
        ax3.plot(max_bins, max_eff[1:], label=label)
        ax4.plot(tot_bins, tot_eff[1:], label=label)
 

    for ax in (ax1, ax2, ax3, ax4):
        ax.xaxis.set_label_coords(.95, -0.1)
        ax.semilogx()
        ax.legend()

    plt.show()


if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Calculate deposited energy and efficiency curves')
    parser.add_argument('rootfiles', nargs='+', help='.root file with simulation data')
    parser.add_argument('-g', '--gain', type=float, default=1, help='ADC counts per electron')
    parser.add_argument('-w', '--white', type=float, help='Maximum pixel response value')
    
    args = parser.parse_args()

    files = [r.TFile(fname) for fname in args.rootfiles]
    hits = {fname: f.Get('hits') for fname, f in zip(args.rootfiles, files)}

    make_plots(hits, gain=args.gain, white=args.white)

    for f in files: f.Close()

    
