#include "include/starflut/starflut_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

#define STARFLUT_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), starflut_plugin_get_type(), \
                              StarflutPlugin))

struct _StarflutPlugin {
  GObject parent_instance;

  FlPluginRegistrar* registrar;

  // Connection to Flutter engine.
  FlMethodChannel* channel;  
};

G_DEFINE_TYPE(StarflutPlugin, starflut_plugin, g_object_get_type())

extern void starflut_plugin_common_handle_method_call(FlPluginRegistrar* registrar,FlMethodChannel* channel,FlMethodCall* method_call);

// Called when a method call is received from Flutter.
static void starflut_plugin_handle_method_call(
    StarflutPlugin* self,
    FlMethodCall* method_call) {
#if 0      
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);

  if (strcmp(method, "getPlatformVersion") == 0) {
    struct utsname uname_data = {};
    uname(&uname_data);
    g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
    g_autoptr(FlValue) result = fl_value_new_string(version);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
#endif
  starflut_plugin_common_handle_method_call(self->registrar,self->channel,method_call);
}

static void starflut_plugin_dispose(GObject* object) {
  StarflutPlugin* self = STARFLUT_PLUGIN(object);

  g_clear_object(&self->registrar);
  g_clear_object(&self->channel);

  G_OBJECT_CLASS(starflut_plugin_parent_class)->dispose(object);
}

static void starflut_plugin_class_init(StarflutPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = starflut_plugin_dispose;
}

extern void starflut_plugin_common_init(FlMethodChannel* in_channel);
static void starflut_plugin_init(StarflutPlugin* self) {}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  StarflutPlugin* plugin = STARFLUT_PLUGIN(user_data);
  starflut_plugin_handle_method_call(plugin, method_call);
}

void starflut_plugin_register_with_registrar(FlPluginRegistrar* registrar) {

 StarflutPlugin* self = STARFLUT_PLUGIN(
      g_object_new(starflut_plugin_get_type(), nullptr));

  self->registrar = FL_PLUGIN_REGISTRAR(g_object_ref(registrar));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  self->channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "starflut",FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(self->channel, method_call_cb,
                                            g_object_ref(self), g_object_unref);
  starflut_plugin_common_init(self->channel);                                         
}
