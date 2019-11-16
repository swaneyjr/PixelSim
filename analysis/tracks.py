#!/usr/bin/env python3

import ROOT as r
import numpy as np
import matplotlib.pyplot as plt

def make_plots(tdict, nplots=2, gain=1, white=None, black=0):
    
    plt.figure(figsize=(16, 12))
    plt.tight_layout()

    axes = []

    ordinals = ['1st', '2nd', '3rd'] + ['{}th'.format(i) for i in range(4,9)]

    for i in range(nplots):
        ax1 = plt.subplot(2, nplots, i+1)
        xlabel = '{} brightest adjacent pix'.format(ordinals[i]) if i else 'Brightest adjacent pix'
        ax1.set_xlabel(xlabel)
        ax1.set_ylabel('Frequency')
        ax1.loglog()
        axes.append(ax1)

    for i in range(nplots):
        ax2 = plt.subplot(2, nplots, nplots+i+1)
        ax2.set_xlabel(xlabel + '/ max')
        ax2.set_ylabel('Frequency')
        ax2.loglog()
        axes.append(ax2)

    if not white: white=np.inf

    for fname, t in tdict.items():

        entries = t.GetEntries()
        e_max = np.zeros(entries)
        e_3x3 = [np.zeros(entries) for _ in range(nplots)]

        label = fname[:-5].replace('_', ' ')

        for ievt, evt in enumerate(t):
            print('{}/{}'.format(ievt+1, entries), end='\r')
            if not len(evt.pix_e): continue

            pix_x = np.array(evt.pix_x)
            pix_y = np.array(evt.pix_y)
            pix_e = np.array(evt.pix_e)

            argsort = np.argsort(pix_e)[::-1]
            xmax = pix_x[argsort[0]]
            ymax = pix_y[argsort[0]]
 
            e_max[ievt] = pix_e[argsort[0]]

            xadj = (pix_x == xmax-1) | (pix_x == xmax+1)
            yadj = (pix_y == ymax-1) | (pix_y == ymax+1)
                
            eadj = np.sort(pix_e[xadj & yadj])[::-1]
            if eadj.size < 8:
                eadj = np.pad(eadj, (0, 8-eadj.size), mode='constant')

            for i in range(nplots):
                e_3x3[i][ievt] = eadj[i]

                
        bins = np.arange(min(white, max(e_max)) + 1)
        rbins = np.geomspace(.001, 1, 100)

        for i in range(nplots):
            hist_3x3 = np.histogram(e_3x3[i], bins=bins)[0]
            hist_r = np.histogram((e_3x3[i]-black)/(e_max-black), bins=rbins)[0]
        
            axes[i].plot((bins[1:]+bins[:-1])/2, hist_3x3, label=label)
            axes[nplots+i].plot((rbins[1:]+rbins[:-1])/2, hist_r, label=label)
        
        print("\n")

    for ax in axes: ax.legend()

    plt.show()

if __name__ == '__main__':
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Compute and plot track statistics')
    parser.add_argument('rootfiles', nargs='+', help='.root files with simulation data')
    parser.add_argument('-n', '--nplots', type=int, default=3, help='Plot the 1st-Nth greatest neighboring pixels')
    parser.add_argument('-g', '--gain', type=float, default=1, help='ADC counts per electron')
    parser.add_argument('-w', '--white', type=int, help='Maximum pixel response value')
    parser.add_argument('-b', '--black', type=int, default=0, help='Black level value')

    args = parser.parse_args()
    files = [r.TFile(fname) for fname in args.rootfiles]
    hits = {fname: f.Get('hits') for fname, f in zip(args.rootfiles, files)}

    make_plots(hits, nplots=args.nplots, gain=args.gain, white=args.white, black=args.black)

