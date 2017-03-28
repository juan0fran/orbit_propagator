## Orbit Propagator for TLE input

# Compilation for GPS constellation

Compile with:

```
gcc main.c gps_tracker_api.c orbit_propagator.c sgdp4.c satutl.c -lm -DDEBUG
```
Simply run: 
```
./a.out
```

# Compilation for Payload Tracker

Compile with:

```
gcc main_payload.c payload_tracker_api.c orbit_propagator.c sgdp4.c satutl.c -lm -DDEBUG
```
Simply run: 
```
./a.out
```

There are no library requirements apart from standard C functions (file open, math library)