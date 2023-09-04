#ifndef _TESTS_VERTEX_WAYLAND_H
#define _TESTS_VERTEX_WAYLAND_H

#include <stdint.h>

#include <wayland-client.h>
#include "xdg-shell.h"

template<typename T>
class WlRegistryListenable
{
public:
    using Global = void(T::*)(struct wl_registry*,
                              uint32_t,
                              const char*,
                              uint32_t);

    using GlobalRemove = void(T::*)(struct wl_registry*,
                                    uint32_t);

public:
    virtual void global(struct wl_registry*,
                        uint32_t,
                        const char*,
                        uint32_t) = 0;
    virtual void global_remove(struct wl_registry*,
                               uint32_t) = 0;

public:
    template<Global global_cb>
    static void global_callback(void *data,
                                struct wl_registry* registry,
                                uint32_t id,
                                const char* interface,
                                uint32_t version)
    {
        ((T*)data->*global_cb)(registry, id, interface, version);
    }

    template<GlobalRemove global_remove_cb>
    static void global_remove_callback(void *data,
                                       struct wl_registry *registry,
                                       uint32_t id)
    {
        ((T*)data->*global_remove_cb)(registry, id);
    }
};

template<typename T>
class XdgWmBaseListenable
{
public:
    using Ping = void(T::*)(struct xdg_wm_base*,
                            uint32_t);

public:
    virtual void ping(struct xdg_wm_base*,
                      uint32_t) = 0;

public:
    template<Ping ping_cb>
    static void ping_callback(void *data,
                              struct xdg_wm_base *xdg_wm_base,
                              uint32_t serial)
    {
        ((T*)data->*ping_cb)(xdg_wm_base, serial);
    }
};

template<typename T>
class XdgSurfaceListenable
{
public:
    using Configure = void(T::*)(struct xdg_surface*,
                                 uint32_t);

public:
    virtual void configure(struct xdg_surface*,
                          uint32_t) = 0;

public:
    template<Configure configure_cb>
    static void configure_callback(void *data,
                                   struct xdg_surface *xdg_surface,
                                   uint32_t serial)
    {
        ((T*)data->*configure_cb)(xdg_surface, serial);
    }
};

template<typename T>
class XdgToplevelListenable
{
public:
    using Configure = void(T::*)(struct xdg_toplevel*,
                                 int32_t,
                                 int32_t,
                                 struct wl_array*);

    using Close = void(T::*)(struct xdg_toplevel*);

public:
    virtual void configure(struct xdg_toplevel*,
                           int32_t,
                           int32_t,
                           struct wl_array*) = 0;

    virtual void close(struct xdg_toplevel*);

public:
    template<Configure configure_cb>
    static void configure_callback(void *data,
                                   struct xdg_toplevel *xdg_toplevel,
                                   int32_t width,
                                   int32_t height,
                                   struct wl_array *states)
    {
        ((T*)data->*configure_cb)(xdg_toplevel, width, height, states);
    }

    template<Close close_cb>
    static void close_callback(void *data,
                               struct xdg_toplevel *xdg_toplevel)
    {
        ((T*)data->*close_cb)(xdg_toplevel);
    }
};

#endif // _TESTS_VERTEX_WAYLAND_H
