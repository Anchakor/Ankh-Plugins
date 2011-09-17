/* LV2Plugin.cpp - Support file for writing LV2 plugins in C++
 * Copyright 2008-2011 David Robillard <d@drobilla.net>
 *
 * Based on lv2plugin.cpp:
 * Copyright 2006-2007 Lars Luthman <lars.luthman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA
 */

#include "LV2Plugin.hpp"


namespace LV2 {

	DescList::~DescList() {
	//	for (unsigned i = 0; i < size(); ++i)
	//		delete [] operator[](i).URI;
    //	drobilla should fix this
	}

	DescList& get_lv2_descriptors() {
		static DescList descriptors;
		return descriptors;
	}

}


extern "C" {

	const LV2_Descriptor* lv2_descriptor(uint32_t index) {
		if (index < LV2::get_lv2_descriptors().size())
			return &LV2::get_lv2_descriptors()[index];
		return NULL;
	}

}


