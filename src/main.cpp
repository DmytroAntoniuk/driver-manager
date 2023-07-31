#include "pch.h"

#include <logs-dir-path.h>

#include "logger-configurator.h"
#include "scanner.h"
#include "state.h"

int main()
{
    Brand::ConfigureLogger();

    Scanner scanner;
    scanner.Start();
}
