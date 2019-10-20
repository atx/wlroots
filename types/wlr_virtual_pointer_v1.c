
#include <assert.h>
#include <stdlib.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_virtual_pointer_v1.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/util/log.h>
#include "util/signal.h"
#include "wlr-virtual-pointer-unstable-v1-protocol.h"

static void input_device_destroy(struct wlr_input_device *dev) {
}

static const struct wlr_input_device_impl input_device_impl = {
	.destroy = input_device_destroy
};

static const struct zwlr_virtual_pointer_v1_interface virtual_pointer_impl;

static struct wlr_virtual_pointer_v1 *virtual_pointer_from_resource(
		struct wl_resource *resource) {
	assert(wl_resource_instance_of(resource,
		&zwlr_virtual_pointer_v1_interface, &virtual_pointer_impl));
	return wl_resource_get_user_data(resource);
}

static void virtual_pointer_motion(struct wl_client *client,
		struct wl_resource *resource, uint32_t time,
		wl_fixed_t dx, wl_fixed_t dy) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	struct wlr_input_device *wlr_dev = &pointer->input_device;
	struct wlr_event_pointer_motion event = {
		.device = wlr_dev,
		.time_msec = time,
		.delta_x = wl_fixed_to_double(dx),
		.delta_y = wl_fixed_to_double(dy),
		.unaccel_dx = wl_fixed_to_double(dx),
		.unaccel_dy = wl_fixed_to_double(dy),
	};
	wlr_signal_emit_safe(&wlr_dev->pointer->events.motion, &event);
}

static void virtual_pointer_motion_absolute(struct wl_client *client,
		struct wl_resource *resource, uint32_t time, uint32_t x, uint32_t y,
		uint32_t x_extent, uint32_t y_extent) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	struct wlr_input_device *wlr_dev = &pointer->input_device;
	struct wlr_event_pointer_motion_absolute event = {
		.device = wlr_dev,
		.time_msec = time,
		.x = (double)x / x_extent,
		.y = (double)y / y_extent,
	};
	wlr_signal_emit_safe(&wlr_dev->pointer->events.motion_absolute, &event);
}

static void virtual_pointer_button(struct wl_client *client,
		struct wl_resource *resource, uint32_t time, uint32_t button,
		uint32_t state) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	struct wlr_input_device *wlr_dev = &pointer->input_device;
	struct wlr_event_pointer_button event = {
		.device = wlr_dev,
		.time_msec = time,
		.button = button,
		.state = state
	};
	wlr_signal_emit_safe(&wlr_dev->pointer->events.button, &event);
}

static void virtual_pointer_axis(struct wl_client *client,
		struct wl_resource *resource, uint32_t time, uint32_t source,
		wl_fixed_t value) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	struct wlr_input_device *wlr_dev = &pointer->input_device;
	struct wlr_event_pointer_axis event = {
		.device = wlr_dev,
		.time_msec = time,
		.source = source,
		.delta = wl_fixed_to_double(value),
		.delta_discrete = wl_fixed_to_int(value),
	};
	wlr_signal_emit_safe(&wlr_dev->pointer->events.axis, &event);
}

static void virtual_pointer_frame(struct wl_client *client,
		struct wl_resource *resource) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	struct wlr_input_device *wlr_dev = &pointer->input_device;
	wlr_signal_emit_safe(&wlr_dev->pointer->events.frame, wlr_dev->pointer);
}

static void virtual_pointer_axis_source(struct wl_client *client,
		struct wl_resource *resource, uint32_t source) {
	// Not supported at the moment
}

static void virtual_pointer_axis_stop(struct wl_client *client,
		struct wl_resource *resource, uint32_t time, uint32_t source) {
	// Not supported at the moment
}

static void virtual_pointer_axis_discrete(struct wl_client *client,
		struct wl_resource *resource, uint32_t time, uint32_t source,
		wl_fixed_t value, int32_t discrete) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	struct wlr_input_device *wlr_dev = &pointer->input_device;
	struct wlr_event_pointer_axis event = {
		.device = wlr_dev,
		.time_msec = time,
		.source = source,
		.delta = wl_fixed_to_double(value),
		.delta_discrete = discrete
	};
	wlr_signal_emit_safe(&wlr_dev->pointer->events.axis, &event);
}

static void virtual_pointer_destroy_resource(struct wl_resource *resource) {
	struct wlr_virtual_pointer_v1 *pointer =
		virtual_pointer_from_resource(resource);
	wlr_signal_emit_safe(&pointer->events.destroy, pointer);
	wl_list_remove(&pointer->link);
	wlr_input_device_destroy(&pointer->input_device);
	free(pointer);
}

static void virtual_pointer_destroy(struct wl_client *client,
		struct wl_resource *resource) {
	wl_resource_destroy(resource);
}

static const struct zwlr_virtual_pointer_v1_interface virtual_pointer_impl = {
	.motion = virtual_pointer_motion,
	.motion_absolute = virtual_pointer_motion_absolute,
	.button = virtual_pointer_button,
	.axis = virtual_pointer_axis,
	.frame = virtual_pointer_frame,
	.axis_source = virtual_pointer_axis_source,
	.axis_stop = virtual_pointer_axis_stop,
	.axis_discrete = virtual_pointer_axis_discrete,
	.destroy = virtual_pointer_destroy,
};

static const struct zwlr_virtual_pointer_manager_v1_interface manager_impl;

static struct wlr_virtual_pointer_manager_v1 *manager_from_resource(
		struct wl_resource *resource) {
	assert(wl_resource_instance_of(resource,
		&zwlr_virtual_pointer_manager_v1_interface, &manager_impl));
	return wl_resource_get_user_data(resource);
}

static void virtual_pointer_manager_create_virtual_pointer(
		struct wl_client *client, struct wl_resource *resource,
		struct wl_resource *seat, uint32_t id) {
	wlr_log(WLR_ERROR, "Creating virtual pointer!!!");
	struct wlr_virtual_pointer_manager_v1 *manager = manager_from_resource(resource);

	struct wlr_virtual_pointer_v1 *virtual_pointer = calloc(1,
		sizeof(struct wlr_virtual_pointer_v1));
	if (!virtual_pointer) {
		wl_client_post_no_memory(client);
		return;
	}

	struct wlr_pointer *pointer = calloc(1, sizeof(struct wlr_pointer));
	if (!pointer) {
		wlr_log(WLR_ERROR, "Cannot allocate wlr_pointer");
		free(virtual_pointer);
		wl_client_post_no_memory(client);
		return;
	}
	wlr_pointer_init(pointer, NULL);

	struct wl_resource *pointer_resource = wl_resource_create(client,
		&zwlr_virtual_pointer_v1_interface, wl_resource_get_version(resource),
		id);
	if (!pointer_resource) {
		free(pointer);
		free(virtual_pointer);
		wl_client_post_no_memory(client);
		return;
	}

	wl_resource_set_implementation(pointer_resource, &virtual_pointer_impl,
		virtual_pointer, virtual_pointer_destroy_resource);

	wlr_input_device_init(&virtual_pointer->input_device,
		WLR_INPUT_DEVICE_POINTER, &input_device_impl, "virtual pointer",
		0x0, 0x0);

	struct wlr_seat_client *seat_client = wlr_seat_client_from_resource(seat);

	virtual_pointer->input_device.pointer = pointer;
	virtual_pointer->resource = pointer_resource;
	virtual_pointer->seat = seat_client->seat;
	wl_signal_init(&virtual_pointer->events.destroy);

	wl_list_insert(&manager->virtual_pointers, &virtual_pointer->link);

	wlr_signal_emit_safe(&manager->events.new_virtual_pointer,
		virtual_pointer);
}

static const struct zwlr_virtual_pointer_manager_v1_interface manager_impl = {
	.create_virtual_pointer = virtual_pointer_manager_create_virtual_pointer
};

static void handle_manager_unbind(struct wl_resource *resource) {
	wl_list_remove(wl_resource_get_link(resource));
}

static void virtual_pointer_manager_bind(struct wl_client *client, void *data,
		uint32_t version, uint32_t id) {
	struct wlr_virtual_pointer_manager_v1 *manager = data;

	struct wl_resource *resource = wl_resource_create(client,
		&zwlr_virtual_pointer_manager_v1_interface, version, id);

	if (!resource) {
		wl_client_post_no_memory(client);
		return;
	}

	wl_resource_set_implementation(resource, &manager_impl, manager,
		handle_manager_unbind);
	wl_list_insert(&manager->resources, wl_resource_get_link(resource));
}

struct wlr_virtual_pointer_manager_v1* wlr_virtual_pointer_manager_v1_create(
		struct wl_display *display)
{
	struct wlr_virtual_pointer_manager_v1 *manager = calloc(1,
		sizeof(struct wlr_virtual_pointer_manager_v1));
	if (!manager) {
		return NULL;
	}

	wl_list_init(&manager->resources);
	wl_list_init(&manager->virtual_pointers);

	wl_signal_init(&manager->events.new_virtual_pointer);
	wl_signal_init(&manager->events.destroy);
	manager->global = wl_global_create(display,
		&zwlr_virtual_pointer_manager_v1_interface, 1, manager,
		virtual_pointer_manager_bind);
	return manager;
}

void wlr_virtual_pointer_manager_v1_destroy(
	struct wlr_virtual_pointer_manager_v1 *manager)
{
	// TODO: Implement
}

