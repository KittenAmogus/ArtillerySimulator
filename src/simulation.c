#include <simulation.h>

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define delta 0.0001

size_t add_projectile_type(simulation_t *sim, projectile_type_t type) {
  if (sim->projectile_types_limit <= sim->projectile_types_count + 1) {
    if (sim->projectile_types_limit <= 0)
      sim->projectile_types_limit = 4;
    sim->projectile_types_limit *= 2;

    sim->projectile_types =
        reallocarray(sim->projectile_types, sim->projectile_types_limit,
                     sizeof(projectile_type_t));
  }

  sim->projectile_types[sim->projectile_types_count] = type;
  return sim->projectile_types_count++;
}

size_t add_projectile(simulation_t *sim, projectile_t proj) {
  if (sim->projectiles_limit <= sim->projectiles_count + 1) {
    if (sim->projectiles_limit <= 0)
      sim->projectiles_limit = 4;
    sim->projectiles_limit *= 2;

    sim->projectiles = reallocarray(sim->projectiles, sim->projectiles_limit,
                                    sizeof(projectile_t));
  }

  proj.id = sim->projectiles_count;
  proj.uid = proj.id;
  sim->projectiles[sim->projectiles_count] = proj;
  return sim->projectiles_count++;
}

size_t create_type(simulation_t *sim, double caliber, double mass,
                   const char *name) {
  projectile_type_t type = {
      .caliber = caliber,
      .mass = mass,
      .name = name,
  };

  printf("Create projectile type [%s] (CAL %.0lf mm, MASS %.02lf)\r\n", name,
         caliber, mass);

  return add_projectile_type(sim, type);
}

size_t create_projectile(simulation_t *sim, size_t type, double yaw,
                         double pitch, double vel) {

  double horizontal_velocity = cos(pitch) * vel;
  double vertical_velocity = sin(pitch) * vel;

  projectile_t projectile = {
      .projectile_type_index = type,

      .x = 0,
      .y = 5,
      .z = 0,

      .vx = cos(yaw) * horizontal_velocity,
      .vy = vertical_velocity,
      .vz = sin(yaw) * horizontal_velocity,
  };

  projectile_type_t *typeptr = &sim->projectile_types[type];

  printf("Create projectile [%s] (YAW %.2lf', PITCH %.1lf') VEL %.2lf m/s\r\n",
         typeptr->name, rad2deg(yaw), rad2deg(pitch), vel);

  return add_projectile(sim, projectile);
}

void on_explosion(simulation_t *sim, projectile_t *proj) {
  projectile_type_t *type = &sim->projectile_types[proj->projectile_type_index];

  double dist = sqrt((proj->x * proj->x) + (proj->z * proj->z));
  printf("[%zu] **EXPLOSION** [%s] (%.1lfm away, %.1lfs) dist (X %.1lfm, Z "
         "%.1lfm)\r\n",
         proj->uid, type->name, dist, sim->simulation_time, proj->x, proj->z);

  // Clear projectile
  size_t new_count = sim->projectiles_count - 1;
  if (new_count != proj->id) {
    sim->projectiles[new_count].id = proj->id;
    memcpy(proj, &sim->projectiles[new_count], sizeof(projectile_t));
  }
  sim->projectiles_count = new_count;
}

int simulation_step(simulation_t *sim) {

  sim->simulation_time += delta;

  for (size_t i = 0; i < sim->projectiles_count; ++i) {

    projectile_t *proj = &sim->projectiles[i];
    projectile_type_t *type =
        &sim->projectile_types[proj->projectile_type_index];

    // Air resistance
    // Fw = 0.5 * cx * ro * S * v^2
    // Aw = Fw/m
    // Ai = -Aw * (vi/v)

    double vel = sqrt(sq(proj->vx) + sq(proj->vy) + sq(proj->vz));

    double caliber_m = type->caliber / 1000.0;
    double S = PI * sq(caliber_m) / 4;

    double M = vel / 340;
    double cx0 = 0.2;
    double cx;

    double sea_t = 15.0;
    double sea_T = 273.15 + sea_t;

    double rho;
    if (proj->y > 0) {
      double T = sea_T - 0.0065 * proj->y;
      double P = 101325.0 * pow((1.0 - (0.0065 * proj->y) / sea_T), 5.25588);
      rho = P / (287.05 * T);
    } else {
      double T = sea_T;
      double P = 101325.0 * pow((1.0 / sea_T), 5.25588);
      rho = P / (287.05 * T);
    }

    if (M < 0.8) {
      cx = cx0;
    } else if (M <= 1.2) {
      cx = cx0 + 2.5 * sq(M - 0.8);
    } else {
      // cx = cx0 * 0.85;
      cx = (cx0 * 2.2) / pow(M, 0.7);
    }

    double force_res = 0.5 * cx * rho * S * sq(vel);

    proj->vx -= ((force_res / type->mass) * (proj->vx / vel)) * delta;
    proj->vy -= ((force_res / type->mass) * (proj->vy / vel)) * delta;
    proj->vz -= ((force_res / type->mass) * (proj->vz / vel)) * delta;

    // Gravity
    proj->vy += -9.81 * delta;

    // Move
    proj->x += (proj->vx * delta);
    proj->y += (proj->vy * delta);
    proj->z += (proj->vz * delta);

    /*
    if (proj->id == 0)
      printf("[UID %zu, ID %zu] (%s) (%.2lf, %.2lf, %.2lf) (%.2lf, %.2lf, "
             "%.2lf)\r\n",
             proj->uid, proj->id, type->name, proj->x, proj->y, proj->z,
             proj->vx, proj->vy, proj->vz);
    */

    // Check collision
    if (proj->y < 0) {
      on_explosion(sim, proj);
    }
  }

  return 0;
}
