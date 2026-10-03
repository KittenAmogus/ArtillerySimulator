#ifndef SIMULATION_H
#define SIMULATION_H

#include <stddef.h>

#define PI 3.1415926
#define deg2rad(deg) (deg * (PI / 180.0))
#define rad2deg(rad) (rad * (180.0 / PI))
#define sq(x) (x * x)

typedef struct {
  double mass;      // Mass of projectile (kg)
  double caliber;   // Caliber of projectile (mm)
  const char *name; // Name of projectile type (short name)
} projectile_type_t;

typedef struct {
  size_t id;                    // Index in array
  size_t uid;                   // Unique unchangeable id
  double x, y, z;               // Offset from center(X, Z), Height(Y) (m)
  double vx, vy, vz;            // Velocity (m/s)
  size_t projectile_type_index; // Projectile type index
} projectile_t;

typedef struct {
  double simulation_time; // Simulation time (ms)
  double real_time;       // Real time (ms)

  // Dynamic array of projectile types
  size_t projectile_types_count;
  size_t projectile_types_limit;
  projectile_type_t *projectile_types;

  // Dynamic array of fired projectiles
  size_t projectiles_count;
  size_t projectiles_limit;
  projectile_t *projectiles;
} simulation_t;

int simulation_step(simulation_t *sim);

// size_t add_projectile_type(simulation_t *sim, projectile_type_t type);
// size_t add_projectile(simulation_t *sim, projectile_t proj);
size_t create_type(simulation_t *sim, double caliber, double mass,
                   const char *name);
size_t create_projectile(simulation_t *sim, size_t type, double yaw,
                         double pitch, double vel);

#endif // SIMULATION_H
