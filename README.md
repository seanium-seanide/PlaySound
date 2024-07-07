# Description

A simple class for playing sound effects. Right now, the class only plays wave
files, which can be loaded and registered with the class. Eventually, the class
will be able to synthesise sinusoids (width optional amplitude and frequency
modulation), square waves, triangular waves, white noise etc, all with
ADSR envelopes.

# Build instructions

```bash
$ # Incremental build
$ make
$
$ # Clean build
$ make clean-build
```

# Run instructions

```bash
$ make run
```

# Clean instructions

```bash
$ # Remove binaries, object files, and their directories
$ make clean
```

# TODO

* Add ability to play sinusoids of given frequency, without reading a wave file from disk
* Add unit tests
* Add simple sound effects, e.g. a bell
* Add e.g. square waves, triangular waves, white noise
