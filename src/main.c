#include <simulation.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

  simulation_t simulation = {0};

  double yaw = 45;
  double pitch = 15;
  double velocity = 800;

  size_t type_HE = create_type(&simulation, 105, 15, "HE_105");
  size_t type_AP = create_type(&simulation, 105, 32, "AP_105");
  size_t type_EMPTY = create_type(&simulation, 105, 4, "EMPTY_105");

  create_projectile(&simulation, type_HE, deg2rad(yaw), deg2rad(pitch),
                    velocity);
  create_projectile(&simulation, type_AP, deg2rad(yaw), deg2rad(pitch),
                    velocity);
  create_projectile(&simulation, type_EMPTY, deg2rad(yaw), deg2rad(pitch),
                    velocity);

  usleep(1000000);

  int running = 1;
  while (running) {
    for (int i = 0; i < 1000; ++i)
      simulation_step(&simulation);

    if (simulation.projectiles_count <= 0) {
      printf("No more projectiles, exit\r\n");
      running = 0;
    }
  }

  return 0;
}
