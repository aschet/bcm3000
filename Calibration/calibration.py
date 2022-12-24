#!/usr/bin/env python3
# Calibration Polynomial Generator for BCM-3000 Beer Color Meter
# Copyright 2022 Thomas Ascher <thomas.ascher@gmx.at>
# SPDX-License-Identifier: MIT

import numpy as np

# Consiting of reference beer name, known EBC units and measured abosrbancy 
# of the reference beer.
CALIBRATION_DATA = [
    [ "Budweiser Budvar Original", 10.0, 0.20942 ],
    [ "Kasteel Rouge",             43.0, 1.29425 ],
    [ "Kasteel Donker",            77.0, 1.61586 ],    
]

def poly_const_to_text(val):
    return 't_double({:.4e})'.format(val)

def poly_to_text(coeff, input_name):
    text = ''
    for i in reversed(coeff[1:]):
        text += poly_const_to_text(i) + '+' + input_name + '*('
    text += poly_const_to_text(coeff[0])
    text = text + ')' * (len(coeff) - 1)
    return text

reference = list(map(lambda x : x[1] / 25.0, CALIBRATION_DATA))
measured = list(map(lambda x : x[2], CALIBRATION_DATA))
coeff = np.polyfit(measured, reference, 2)
print(poly_to_text(coeff, 'abs'))
