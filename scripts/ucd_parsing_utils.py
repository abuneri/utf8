PROP_CPP_CONV = {
    'CR': 'property::CR',
    'LF': 'property::LF',
    'Control': 'property::Control',
    'Extend': 'property::Extend',
    'Regional_Indicator': 'property::RI',
    'Prepend': 'property::Prepend',
    'SpacingMark': 'property::SpacingMark',
    'L': 'property::L',
    'V': 'property::V',
    'T': 'property::T',
    'LV': 'property::LV',
    'LVT': 'property::LVT',
    'ZWJ': 'property::ZWJ',
    'Emoji': 'property::Emoji',
    'Emoji_Presentation': 'property::Emoji_Pres',
    'Emoji_Modifier': 'property::Emoji_Mod',
    'Emoji_Modifier_Base': 'property::Emoji_Mod_Base',
    'Emoji_Component': 'property::Emoji_Comp',
    'Extended_Pictographic': 'property::Ext_Pict'
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
    props = []
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
                props.append(Property(cp, prop_type))

    return props
