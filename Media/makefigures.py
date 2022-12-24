#!/usr/bin/env python3
# Copyright 2022 Thomas Ascher <thomas.ascher@gmx.at>
# SPDX-License-Identifier: MIT

import numpy as np
import matplotlib.pyplot as plt

srm_values = np.array(range(0, 51))

absorbance_10mm = srm_values / 12.7
absorbance_5mm = absorbance_10mm * 0.5

def calc_transmission(absorbance):
    return np.power(np.full(shape=absorbance.shape, fill_value=10.0), absorbance * -1.0) * 100.0

transmission_10mm = calc_transmission(absorbance_10mm)
transmission_5mm = calc_transmission(absorbance_5mm)

fig = plt.figure(constrained_layout=True, figsize=(4, 4))
axes = fig.subplots(1, 1)
axes.set_xlabel('SRM')
axes.set_ylabel('T%')
axes.plot(srm_values, transmission_10mm, label='1.0 cm path length ')
axes.plot(srm_values, transmission_5mm, label='0.5 cm path length')
axes.set_xticks(ticks=np.arange(0, len(transmission_10mm), 5), dtype=int)
axes.set_yticks(ticks=np.arange(0, 101, 10), dtype=int)
axes.legend()
axes.set_title('Transmission Rates Over SRM Scale')
fig.savefig('transmission.png', format='png')

#plt.show()