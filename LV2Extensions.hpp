/* LV2Extensions - Extension Mixins for LV2Plugin C++ wrapper class
 * Copyright 2011      David Robillard <d@drobilla.net>
 * Copyright 2006-2007 Lars Luthman <lars.luthman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA
 */

#ifndef LV2EXTENSIONS_HPP
#define LV2EXTENSIONS_HPP

#include "LV2Plugin.hpp"
#include "lv2/lv2plug.in/ns/ext/contexts/contexts.h"
#include "lv2/lv2plug.in/ns/ext/resize-port/resize-port.h"
#include "lv2/lv2plug.in/ns/ext/uri-map/uri-map.h"
#include "lv2/lv2plug.in/ns/ext/uri-unmap/uri-unmap.h"

namespace LV2 {
namespace Ext {

/** The fixed buffer size extension. A host that supports this will always
 *  call the plugin's run() function with the same @c sample_count parameter,
 *  which will be equal to the uint32_t variable pointed to by the data
 *  pointer for this feature. */
template<bool Required>
struct FixedSize {
	template<class Derived>
	struct I : Extension<Required> {
		I() : m_buffer_size(0) {}

		static void map_feature_handlers(FeatureHandlerMap& hmap) {
			hmap["http://tapas.affenbande.org/lv2/ext/fixed-buffersize"]
			    = &I<Derived>::handle_feature;
		}

		static void handle_feature(void* instance, void* data) {
			Derived*    d  = reinterpret_cast<Derived*>(instance);
			I<Derived>* fe = static_cast<I<Derived>*>(d);
			fe->m_buffer_size = *reinterpret_cast<uint32_t*>(data);
			fe->m_ok          = true;
		}

	protected:
		/** This returns the buffer size that the host has promised to use.
		 *  If the host does not support this extension this function will
		 *  return 0. */
		uint32_t get_buffer_size() const { return m_buffer_size; }

		uint32_t m_buffer_size;
	};
};

/** The fixed power-of-2 buffer size extension. This works just like
 *  FixedExt with the additional requirement that the buffer size must
 *  be a power of 2. */
template<bool Required>
struct FixedP2Size {
	template<class Derived>
	struct I : Extension<Required> {
		I() : m_buffer_size(0) {}

		static void map_feature_handlers(FeatureHandlerMap& hmap) {
			hmap["http://tapas.affenbande.org/lv2/ext/power-of-two-buffersize"]
					= &I<Derived>::handle_feature;
		}

		static void handle_feature(void* instance, void* data) {
			Derived*    d  = reinterpret_cast<Derived*>(instance);
			I<Derived>* fe = static_cast<I<Derived>*>(d);
			fe->m_buffer_size = *reinterpret_cast<uint32_t*>(data);
			fe->m_ok          = true;
		}

	protected:
		/** This returns the buffer size that the host has promised to use.
		 *  If the host does not support this extension this function will
		 *  return 0. */
		uint32_t get_buffer_size() const { return m_buffer_size; }

		uint32_t m_buffer_size;
	};
};


/** The URI map extension. */
template <bool Required>
struct UriMap {
	template <class Derived> struct I : Extension<Required> {
		I() : m_uri_map_feature(0) {}

		static void map_feature_handlers(FeatureHandlerMap& hmap) {
			hmap[LV2_URI_MAP_URI] = &I<Derived>::handle_feature;
		}

		static void handle_feature(void* instance, void* data) {
			Derived* d = reinterpret_cast<Derived*>(instance);
			I<Derived>* fe = static_cast<I<Derived>*>(d);
			fe->m_uri_map_feature = reinterpret_cast<LV2_URI_Map_Feature*>(data);
			fe->m_ok = true;
		}

	protected:
		uint32_t uri_to_id(const char* map, const char* uri) const {
			return m_uri_map_feature->uri_to_id(
					m_uri_map_feature->callback_data, map, uri);
		}

		LV2_URI_Map_Feature* m_uri_map_feature;
	};
};


/** The URI unmap extension. */
template <bool Required>
struct UriUnmap {
	template <class Derived> struct I : Extension<Required> {
		I() : m_uri_unmap_feature(0) {}

		static void map_feature_handlers(FeatureHandlerMap& hmap) {
			hmap[LV2_URI_UNMAP_URI] = &I<Derived>::handle_feature;
		}

		static void handle_feature(void* instance, void* data) {
			Derived* d = reinterpret_cast<Derived*>(instance);
			I<Derived>* fe = static_cast<I<Derived>*>(d);
			fe->m_uri_unmap_feature = reinterpret_cast<LV2_URI_Unmap_Feature*>(data);
			fe->m_ok = true;
		}

	protected:
		const char* id_to_uri(const char* map, uint32_t id) const {
			return m_uri_unmap_feature->id_to_uri(
					m_uri_unmap_feature->callback_data, map, id);
		}

		LV2_URI_Unmap_Feature* m_uri_unmap_feature;
	};
};


/** Message Context
 * Plugin must have method:
 * uint32_t message_run(const void* valid_inputs,
 *                      void*       valid_outputs);
 */
template <bool Required>
struct MessageContext {
	template <class Derived> struct I : Extension<Required> {
		I() {}

		static const void* extension_data(const char* uri) {
			static LV2_Contexts_MessageContext context;
			context.run = &Derived::_message_run;
			return &context;
		}

	protected:
		static uint32_t _message_run(LV2_Handle  instance,
		                             const void* valid_inputs,
		                             void*       valid_outputs) {
			return reinterpret_cast<Derived*>(instance)->message_run(valid_inputs, valid_outputs);
		}
		
	};
};


/** Resize port extension. */
template <bool Required>
struct ResizePort {
	template <class Derived> struct I : Extension<Required> {
		I() : m_resize_port_feature(0) {}

		static void map_feature_handlers(FeatureHandlerMap& hmap) {
			hmap[LV2_RESIZE_PORT_URI] = &I<Derived>::handle_feature;
		}

		static void handle_feature(void* instance, void* data) {
			Derived* d = reinterpret_cast<Derived*>(instance);
			I<Derived>* fe = static_cast<I<Derived>*>(d);
			fe->m_resize_port_feature = reinterpret_cast<LV2_Resize_Port_Feature*>(data);
			fe->m_ok = true;
		}

		static bool resize_port(void* instance, uint32_t index, size_t size) {
			Derived* d = reinterpret_cast<Derived*>(instance);
			return d->m_resize_port_feature->resize_port(
					d->m_resize_port_feature->data, index, size);
		}

	protected:
		LV2_Resize_Port_Feature* m_resize_port_feature;
	};
};


} // namespace Ext
} // namespace LV2

#endif // LV2EXTENSIONS_HPP
