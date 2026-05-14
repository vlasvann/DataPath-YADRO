#include "app.h"

int main(int argc, char *argv[]) {
  App app;

  if (!app.Init(argc, argv)) {
    return 1;
  }

  return app.Run();
}
