//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <starflut/starflut_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) starflut_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "StarflutPlugin");
  starflut_plugin_register_with_registrar(starflut_registrar);
}
