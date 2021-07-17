#!/usr/bin/env python

import os

import numpy as np
import matplotlib.pyplot as plt
import ROOT as r

from argparse import ArgumentParser

from efficiency import get_total_events

cmap = plt.get_cmap('tab20c')

def get_effs(fname, threshvals):
    f = r.TFile(fname)
    hits = f.Get('hits')

    n_tot = get_total_events(f)
    #print(n_tot)
    #print(hits.GetEntries())
    n_thresh = [hits.GetEntries('n_tot >= {}'.format(t)) for t in threshvals]
    #print(n_thresh)

    return np.array(n_thresh) / n_tot, np.sqrt(n_thresh) / n_tot

parser = ArgumentParser()

parser.add_argument('infiles', nargs='+', help='Output of gamma_scan.sh')
parser.add_argument('--thresh', nargs='+', required=True, help='Thresholds in electrons to plot')
parser.add_argument('--small', action='store_true', help='Generate a small plot')
args = parser.parse_args()

energy = []
efficiencies = []
errors = []
for f in args.infiles:
    keV = int(os.path.splitext(os.path.basename(f))[0])
    energy.append(keV/1000)
    print(keV)
    eff_e, err_e = get_effs(f, args.thresh)
    efficiencies.append(eff_e)
    errors.append(err_e)

figsize = (4.5,3.2) if args.small else (7,5)
plt.figure(figsize=figsize, tight_layout=True)
ax = plt.gca()

esort = np.argsort(energy)
energy = np.sort(energy)
eff = np.transpose(efficiencies)[:, esort]
err = np.transpose(errors)[:, esort]
for i in range(len(args.thresh)):
    #ax.errorbar(energy, eff[i], yerr=err[i], color=cmap(8+i), label='{} $e^-$'.format(args.thresh[i]))
    plt.plot(energy, eff[i], color=cmap(8+i), label='{} $e^-$'.format(args.thresh[i]))

ax.set_xlabel(r'$E$ [MeV]')
ax.set_ylabel('Efficiency')
plt.semilogx()
plt.semilogy()
ax.legend()

plt.show()
