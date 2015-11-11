/*
 * Filter Pro Plugin for Audacious
 * Copyright 2015-2016 Andrey Karpenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions, and the following disclaimer in the documentation
 *    provided with the distribution.
 *
 * This software is provided "as is" and without any warranty, express or
 * implied. In no event shall the authors be liable for any damages arising from
 * the use of this software.
 */

#include <libaudcore/i18n.h>
#include <libaudcore/runtime.h>
#include <libaudcore/plugin.h>
#include <libaudcore/preferences.h>
#include <libaudcore/audstrings.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


enum
{
    FILTERPRO_PRESET1			= 0,
    FILTERPRO_PRESET2			= 1,
    FILTERPRO_PRESET3			= 2,
    FILTERPRO_PRESET_NUM
} ;


class FilterPro : public EffectPlugin
{
public:
    static const char about[];
    static const char * const defaults[];
    static const PreferencesWidget widgets[];
    static const PluginPreferences prefs;

    static constexpr PluginInfo info = {
        N_("Filter Pro"),
        PACKAGE,
        about,
        & prefs
    };

    /* order #2: must be before crossfade */
    constexpr FilterPro () : EffectPlugin (info, 2, false) {}

    bool init ();
    void cleanup ();

    void start (int & channels, int & rate);
    bool flush (bool force);

    Index<float> & process (Index<float> & data)
        { return filterprocess (data, false); }
    Index<float> & finish (Index<float> & data, bool end_of_playlist)
        { return filterprocess (data, true); }

private:
    Index<float> & filterprocess (Index<float> & data, bool finish);
};

EXPORT FilterPro aud_plugin_instance;

const char * const FilterPro::defaults[] = {
 "filter", aud::numeric_string<FILTERPRO_PRESET1>::str,
 "preset_1", "",
 "preset_2", "",
 "preset_3", "",
 nullptr};

static int    filter;
static String filterPath;
static Index<float> buffer;
static int stored_channels;
static int current_rate;

bool FilterPro::init ()
{
    aud_config_set_defaults ("filterpro", defaults);
    return true;
}

void FilterPro::cleanup ()
{
	/* TODO: add clean up if needed */

    buffer.clear ();
}

int filter_proc(float* in_buffer, float* out_buffer, size_t samples_num, uint32_t channel_num, bool finish)
{
	return 0;
}

int filter_flush()
{
	return 0;
}

int filter_init(uint32_t channel_num, uint32_t rate, const char* config, uint32_t configLen)
{
	return 0;
}

void FilterPro::start (int & channels, int & rate)
{
	/* TODO: add clean up if needed */

    int current_filter = aud_get_int ("filterpro", "filter");

  	const char* paramterName;
    switch(current_filter)
    {
    case FILTERPRO_PRESET1: paramterName = "preset_1"; break;
    case FILTERPRO_PRESET2: paramterName = "preset_2"; break;
    case FILTERPRO_PRESET3: paramterName = "preset_3"; break;
    default:
    	AUDERR ("Invalid preset index, moving to default preset_1\n");
    	paramterName = "preset_1";
    }

    filterPath = aud_get_str ("filterpro", paramterName);

    VFSFile input_file;
    if (! VFSFile::test_file (filterPath, VFS_EXISTS))
    {
    	input_file = VFSFile (filterPath, "r");
    	Index<char> bufferpp = input_file.read_all ();

    	if(filter_init(channels, rate, bufferpp.begin(), bufferpp.len()))
    	{
    		AUDERR ("Filter initialization failed\n");
    	}
    }
    else
    {
    	AUDERR ("Unable to read filter configuration\n");
    	if(filter_init(channels, rate, nullptr, 0))
       	{
       		AUDERR ("Filter initialization failed\n");
       	}
    }

    filter = current_filter;
    stored_channels = channels;
    current_rate = rate;
}

Index<float> & FilterPro::filterprocess (Index<float> & data, bool finish)
{
    if (! data.len ())
        return data;

    buffer.resize ((int) (data.len ()));

    if(filter_proc(data.begin(), buffer.begin(), data.len()/stored_channels, stored_channels, finish))
    {
    	AUDERR ("Filter processing failed\n");
    }

    if (finish)
        flush (true);

    return buffer;
}

bool FilterPro::flush (bool force)
{
	if(filter_flush())
	{
		AUDERR ("Filter flush failed\n");
		return false;
	}

    return true;
}

const char FilterPro::about[] =
 N_("Filter Pro Plugin for Audacious\n"
    "Copyright 2015-2016 Andrey Karpenko");

static const ComboItem filter_list[] = {
	    ComboItem(N_("Preset 1"), FILTERPRO_PRESET1),
	    ComboItem(N_("Preset 2"), FILTERPRO_PRESET2),
	    ComboItem(N_("Preset 3"), FILTERPRO_PRESET3),
};

const PreferencesWidget FilterPro::widgets[] = {
    WidgetLabel (N_("<b>Filter Pro settings</b>")),
    WidgetCombo (N_("Active configuration:"),
        WidgetInt ("filterpro", "filter"),
        {{filter_list}}),
    WidgetLabel (N_("<b>Filter configuration</b>")),
    WidgetEntry (N_("Preset 1:"),
    	WidgetString ("filterpro", "preset_1"),
        {false},
        WIDGET_CHILD),
    WidgetEntry (N_("Preset 2:"),
    	WidgetString ("filterpro", "preset_2"),
        {false},
        WIDGET_CHILD),
    WidgetEntry (N_("Preset 3:"),
    	WidgetString ("filterpro", "preset_3"),
        {false},
        WIDGET_CHILD),
};

const PluginPreferences FilterPro::prefs = {{widgets}};
