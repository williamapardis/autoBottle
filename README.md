# autoBottle
A automated solution dispensing system. A electromechanical integration of a [Fisherbrand™ Reusable Glass Media Bottles](https://www.fishersci.com/shop/products/fisherbrand-reusable-glass-media-bottles-cap-13/FB8001000) and a [Atlas Scientific EZO-PMP™](https://atlas-scientific.com/kits/ezo-pmp-kit/) parastaltic pump through a [laser cut clamp/adapter](https://github.com/williamapardis/autoBottle/blob/main/hardware/laserCut/autoBottleClamp.dxf). The bottles come in various volumes (0.1-10 [L]) having interchangable Caps therefore making the autoBottle system consistent with any of the available volumes.

<img alt="image" src="https://github.com/user-attachments/assets/e09fcb53-921f-4617-b79e-99e7190f0efe" />

## Pump Setup
The pump requires minimal hardware configuration and setup for appropriate use.

### Cabling
The [stock cable](https://atlas-scientific.com/peristaltic/ezo-data-cable/) from Atlas Scientific (Molex part [#51065-0500](https://www.digikey.com/en/products/detail/molex/0510650500/403569)) is terminated with a [qwiic standard I2C interface](https://www.sparkfun.com/qwiic?gad_source=1&gad_campaignid=17479024030&gbraid=0AAAAADsj4ERiL2Oc8jgb2E0PI90LBolKh&gclid=CjwKCAjwzevPBhBaEiwAplAxviI5-nGhxGdM6zCEWpxVcGYV_PpUcOE8D-CgIUTyClchAJz0VKG5PBoCs-AQAvD_BwE) ([JST SH Series](https://www.digikey.com/en/products/detail/jst-sales-america-inc/SHR-04V-S-B/759868)). 

<img width="12560" height="5920" alt="image" src="https://github.com/user-attachments/assets/b6b934ae-39b3-49a8-994c-cd94c3906aa9" />

### I2C Mode

### Calibration

## Eventually
What I would like eventaully is a library where you can instantiate a pump class. somthing like:

'''
pump stockPump(addr, dv, dt);
'''

addr = pump address
dv   = volumes to dispense
dt   = intervals between dispensing
