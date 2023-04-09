PROP_CPP_CONV = {
    'CR': 'property::type::CR',
    'LF': 'property::type::LF',
    'Control': 'property::type::Control',
    'Extend': 'property::type::Extend',
    'Regional_Indicator': 'property::type::RI',
    'Prepend': 'property::type::Prepend',
    'SpacingMark': 'property::type::SpacingMark',
    'L': 'property::type::L',
    'V': 'property::type::V',
    'T': 'property::type::T',
    'LV': 'property::type::LV',
    'LVT': 'property::type::LVT',
    'ZWJ': 'property::type::ZWJ',
    'Emoji': 'property::type::Emoji',
    'Emoji_Presentation': 'property::type::Emoji_Pres',
    'Emoji_Modifier': 'property::type::Emoji_Mod',
    'Emoji_Modifier_Base': 'property::type::Emoji_Mod_Base',
    'Emoji_Component': 'property::type::Emoji_Comp',
    'Extended_Pictographic': 'property::type::Ext_Pict'
}


def codepoint_range(codepoint):
    if '..' in codepoint:
        start = 0
        end = 0

        if len(codepoint) == 10:
            # U+XXXX
            start = int(codepoint[0:4], base=16)
            end = int(codepoint[6:10], base=16)
        elif len(codepoint) == 12:
            # U+XXXXX
            start = int(codepoint[0:5], base=16)
            end = int(codepoint[7:12], base=16)

        if start == end or start > end:
            return None

        return [cp for cp in range(start, end + 1)]
    else:
        return [int(codepoint, base=16)]


def get_properties(ucd_file, Property):
    props = {}
    with open(ucd_file, 'r', encoding='utf-8') as gpb_file:
        lines = gpb_file.readlines()
        for line in lines:
            if line.startswith('#') or not line:
                continue
            first_section = line.split(';')
            if len(first_section) != 2:
                continue

            codepoint = first_section[0].strip()
            codepoints = codepoint_range(codepoint)
            if codepoints is None:
                continue

            second_section = first_section[1].split('#')
            if len(second_section) != 2:
                continue

            prop_type = second_section[0].strip()
            for cp in codepoints:
                if cp in props.keys():
                    prop = props[cp]
                    if hasattr(prop, 'append'):
                        prop.append(prop_type)
                else:
                    props[cp] = Property(cp, prop_type)

    return list(props.values())
