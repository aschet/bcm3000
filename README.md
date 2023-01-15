# BCM-3000: Beer Color Meter (Work in Progress)

While measuring specific gravity is ubiquitous in home brewing, measuring beer color in EBC/SRM units is not. One reason for this is the cost associated with the required laboratory equipment. For this reason, this project aims to develop a low-cost LED-based specialty photometer that implements the procedures defined in the <a href="https://www.mebak.org">MEBAK</a> II 2.12 and <a href="https://www.asbcnet.org">ASBC</a> Beer-10 analytical methods. All design files for the Arduino-based BCM-3000, including the <a href="https://www.kicad.org">KiCad</a> schematic and <a href="https://www.freecadweb.org">FreeCAD</a> CAD drawings for a 3D printable cuvette holder, have been published in this repository so that you can build this instrument yourself. This project is not the first attempt to create an LED-based photometer to measure beer color. Di Caro et al. (2019) and Barth et al. (2021) have already built similar devices.

<p align="center">
  <img src="Media/assembly.jpg?raw=true">
</p>

## Measurement of Beer Color

Beer color, or more precisely the intensity of beer color, is measured with a high-precision spectrophotometer (accuracy of at least +-0.5 nm) based on the Beer-Lambert law. In a measurement, the light absorption of the sample at a wavelength of 430 nm is determined against (distilled) water through a square cuvette with a path length of 10 mm. The samples must be degassed and filtered for turbidity with a membrane filter (0.45 µm) before measurement. If the filtration does not result in a clear sample, additional filtration steps must be performed, e.g. by adding 0.1 % kieselguhr to the sample and repeating the filtration. The measurement result is calculated using the following two formulas, where A is the measured absorbance at 430 nm and D is the dilution factor. Samples of dark beers may need to be diluted before measurement, otherwise they will not be measurable within the linear range of an instrument. A dilution of 1:1 corresponds to a D factor of 2. The factor 1.27 scales the absorbance from a cuvette with a path length of 10 mm to a half-inch cuvette. EBC units should be specified with two significant figures, while SRM units should be specified as degrees to one decimal place.

$$\textrm{EBC} = 25 \cdot A_{430} \cdot D$$ 

$$\textrm{SRM} = 10 \cdot 1.27 \cdot A_{430} \cdot D$$ 

The absorbance can be calculated from the measured light transmission. To determine the transmittance value, you need to measure the intensity when no light hits the detector (noise) and then when light shines through water and the sample to be measured.

$$\textrm{A} =  -\log(T)$$ 

$$\textrm{T} =  \frac{I_{\textrm{Beer}} - I_{\textrm{Noise}}}{I_{\textrm{Water}} - I_{\textrm{Noise}}}$$ 

In addition to the described analysis method, the ASBC Beer-10 also defines the tristimulus method, which requires the measurement of the entire visible light spectrum from which a unambiguous CIELAB color can be calculated. This method is not supported by the BCM-3000.

### LEDs and Calibration of Photometers

The monochromator in a spectrophotometer, e.g. an optical grating, splits the light into its components. With additional optical components, the operator can then select a specific wavelength for measurement. Similarly, LEDs can be manufactured to emit a specific peak wavelength. It should be noted that LEDs do not emit monochromatic light and may also vary within the allowable product tolerance due to the manufacturing process. Calibration and adjustment is required to compensate for these differences. Beer-10 provides a calibration procedure for photometers with a moderately wide spectral bandpass filter.

For calibration, use a filter that is transmissive in the range of 420-450 nm. Select cuvettes with a specific path length that give absorbance values between 0.187 and 0.699 (20-65% transmission) for better accuracy. Measure beers with multiple color levels using a high-precision spectrophotometer and also the photometer with a bandpass filter. Measure at least two samples in each color level. Then calculate the average value for both instruments in each level. A calibration curve can be derived from this data.

### Turbidity Detection (Not Implemented)

In addition to the procedure described above, the ASBC establishes a criterion based on an additional absorbance measurement at a wavelength of 700 nm to determine whether a beer sample is "free of turbidity" and thus suitable for measurement:

$$A_{700} \le 0.039 \cdot A_{430}$$

### Turbidity Compensation (Not Implemented)

Although not part of a standard, several sources suggest a method to compensate for turbidity based on an absorbance measurement at a wavelength of 700 nm, since sample preparation is not always possible, e.g. for inline measurements. Please note that these are quite large corrections. "Color of Wort" (1954) suggests the following formula where optical density has been replaced by absorbance:

$$A_{430}' = A_{430} - (A_{700} - 0.039 \cdot A_{430})$$

The Chemtronic Waltemode GmbH (2012) describes a similar approach:

$$A_{430}' = A_{430} - A_{700}$$

### Carbon Dioxide Detection (Not Implemented)

To determine whether carbonization has actually been removed from a sample, Di Caro et al. (2019) measure the change in radiation on the detector side. The presence of residual carbon dioxide results in a change in the absorbed radiation. Unfortunately, they do not give a threshold for detection.

### Extended Measurement Range (Not Implemented)

At about 30 SRM or about 60 EBC, most of the light transmitted through a sample in a cuvette with 10 mm path length is absorbed at a wavelength of 430 nm. The sample must then be diluted to obtain a meaningful measurement result. There are several measures that can be taken to measure darker beers without dilution, although these are not standard procedures. A cuvette with a shorter path length, such as 5 mm, will result in lower absorbance (measurement results must be scaled to 10 mm), but cuvettes with a path length of less than 10 mm are typically much more expensive. To obtain more accurate inline measurement results for very dark beers (up to 35 EBC), Chemtronic Waltemode GmbH (2012) suggests taking measurements at a wavelength of 550 nm instead of 430 nm because the light absorption is lower. However, a model to correlate with the absorbance at 430 nm is required. The British I.O.B. initially performed measurements at a wavelength of 530 nm before 430 nm was introduced (Seaton et al., 1993).

<p align="center">
  <img src="Media/transmission.png?raw=true">
</p>

## Usage of the BCM-3000

* For adjustment, place a cuvette filled with (distilled) water in the cuvette holder. The transparent cuvette window should point in the direction of the triangular mark on the cuvette holder. Place the cuvette holder cap on the cuvette holder and press button 2 to start the adjustment. When adjustment is complete, the ADJ status is activated on the instrument LCD. If you press and hold the button 2 for one second, the adjustment is reset.
* For measurement, place a cuvette filled with decarbonated and non turbid beer in the cuvette holder. Place the cuvette holder cap on the cuvette holder and press button 1 to start the measurement. When the measurement is complete, the result value is displayed on the LCD of the instrument. Pressing and holding the button 1 for one second switches between EBC and SRM units.
* The holder of the BCM-3000 accepts square cuvettes with a path length of 10 mm and a height of 45 mm. The cuvettes should be suitable for visible light spectroscopy and therefore made of glass, polystyrene (PS) or polymethyl methacrylate (PMMA).

<p align="center">
  <img src="Media/cuvette.png?raw=true">
</p>

## Schematic

A transimpedance amplifier (TIA) was integrated into the circuit for signal processing. The design and calculations for the TIA were derived from a reference design by Caldwell, J. (2014).

## Bill of Materials

| Description                                     | Qty |
| ---                                             | --- |
| Arduino Nano                                    | 1   |
| Mini USB cable                                  | 1   |
| USB power adapter                               | 1   |
| Everlight 333-2UBC/C430 LED                     | 1   |
| Osram SFH 203 photodiode                        | 1   |
| Microchip MCP6002-I/P op amp                    | 1   |
| Joy-IT 16x2 LCD module                          | 1   |
| Push button                                     | 2   |
| Board                                           | 1   |
| Soldering and wiring material                   | 1   |
| 22 pF capacitor                                 | 1   |
| 60 Ω resistor                                   | 1   |
| 270 Ω resistor                                  | 1   |
| 14 kΩ resistor                                  | 1   |
| 560 kΩ resistor                                 | 2   |
| Custom cuvette holder                           | 1   |
| Custom cuvette holder cap                       | 1   |
| Housing                                         | 1   |
| Square cuvette, 10 mm path length, 45 mm height | 1   |

## Open Issues and Limitations

* Currently, the instrument does not provide satisfactory measurement results. The darker a beer is, the higher the deviation from the expected measured value.
* Integration of optical components with a narrower spectral bandwidth.
* Calibration procedure to compensate for the spectral characteristics of the LED used.
* Evaluation of the accurracy and repeatabillity as well as the suitable measurement range.
* For diluted samples, the dilution factor itself must be included in the measurement result.
* The instrument does not have a temperature compensation.

## References

* <a href="https://doi.org/10.1080/03610470.2020.1807888">Barth, R., Rieger, R. H., & Kim, Y. (2021). Ruggedized color measurement for beer, wort, and malt. Journal of the American Society of Brewing Chemists. American Society of Brewing Chemists, 79(1), 41-45.</a>
* <a href="https://www.ti.com/lit/pdf/tidu535">Caldwell, J. (2014). 1 MHz, Single-Supply, Photodiode Amplifier Reference Design.</a>
* <a href="https://www.chemtronic-gmbh.de/images/chemtronic/Apps_e_pdf/EBC%20Colour%20I.pdf">Chemtronic Waltemode GmbH. (2012). EBC-Colour Measurement.</a>
* <a href="https://doi.org/10.1080/00960845.1954.12006433">Color of Wort. (1954). Proceedings Annual Meeting - American Society of Brewing Chemists, 12(1), 168–170.</a>
* <a href="https://doi.org/10.1109/MetroAgriFor.2019.8909213">Di Caro, D., Liguori, C., Pietrosanto, A., & Sommella, P. (2019). A low-cost device for beer color measurement. 2019 IEEE International Workshop on Metrology for Agriculture and Forestry (MetroAgriFor).</a>
* <a href="https://doi.org/10.1002/j.2050-0416.1993.tb01140.x">Seaton, J. C., Cantrell, I. C., & Submitted on Behalf of the institute of Brewing Analysis Committee. (1993). The determination of beer colour-collaborative trial. Journal of the Institute of Brewing. Institute of Brewing (Great Britain), 99(1), 21–23.</a>
* <a href="https://www.asbcnet.org/Methods/Pages/default.aspx">Technical Committe, A. (2011). Color. In ASBC Methods of Analysis. American Society of Brewing Chemists.</a>