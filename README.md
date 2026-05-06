# autoBottle
A automated solution dispensing system. A electromechanical integration of a [Fisherbrand™ Reusable Glass Media Bottles](https://www.fishersci.com/shop/products/fisherbrand-reusable-glass-media-bottles-cap-13/FB8001000) and a [Atlas Scientific EZO-PMP™](https://atlas-scientific.com/kits/ezo-pmp-kit/) parastaltic pump through a [laser cut clamp/adapter](https://github.com/williamapardis/autoBottle/blob/main/hardware/laserCut/autoBottleClamp.dxf). The bottles come in various volumes (0.1-10 [L]) having interchangable Caps therefore making the autoBottle system consistent with any of the available volumes.

<img width="2000" height="2600" alt="image" src="https://github.com/user-attachments/assets/e09fcb53-921f-4617-b79e-99e7190f0efe" />

## Pump Setup
### Cabling
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
