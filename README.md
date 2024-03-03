# Radiation Transmission Studies

Compute the radiation transport of a particle through a medium. Counts secondaries produced.

## Usage

```bash
Usage: ./radiation-transmission [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--primaries INT:POSITIVE REQUIRED
                              Number of primary particles to launch
  -t,--threads INT:POSITIVE   Number of threads
  -p,--particle TEXT:{neutron,gamma,proton,electron,muon} REQUIRED
                              Input particle type
  -i,--input TEXT REQUIRED    Input root filename with particle energy / angle information
  -o,--output TEXT REQUIRED   Output root filename
  -d,--detector [TEXT,FLOAT] ... REQUIRED
                              Detector configuration: material and thickness (in mm) in the following format: '-d G4_Pb 100'. If called multiple times they will be stacked
```
