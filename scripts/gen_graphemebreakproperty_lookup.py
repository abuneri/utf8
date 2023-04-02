from ucd_parsing_utils import codepoint_range, PROP_CPP_CONV


class GraphemeBreakProperty:
    def __init__(self, codepoint, prop_type):
        self.codepoint = codepoint
        self.prop_type = PROP_CPP_CONV[prop_type]

def get_properties():
    props = []
    with open('GraphemeBreakProperty.txt', 'r', encoding='utf-8') as gpb_file:
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
                props.append(GraphemeBreakProperty(cp, prop_type))

    return props


def get_cpp_unordermap_data(props):
    start_header_data = \
        '// NOTE: This file was autogenerated via ' \
        'scripts/gen_graphemebreakproperty_lookup.py\n' \
        '#pragma once\n\n' \
        '#include <unordered_map>\n\n' \
        '#include <auc/codepoint.hpp>\n\n' \
        '#include "property.hpp"\n\n' \
        '// https://www.unicode.org/' \
        'Public/15.0.0/ucd/auxiliary/GraphemeBreakProperty.txt\n'\
        'namespace auc {\n' \
        'namespace detail {\n\n' \
        'struct grapheme_cluster_break {\n' \
        '   codepoint codepoint_{0};\n'\
        '   property prop_{property::Other};\n' \
        '};\n\n' \
        'static std::unordered_map<std::uint32_t, grapheme_cluster_break> ' \
        'codepoint_break_lookup = {\n'

    header_data = ''
    num_props = len(props)
    for prop_idx in range(num_props):
        prop = props[prop_idx]
        header_data += \
            f'  {{{prop.codepoint}u, {{{prop.codepoint}u, {prop.prop_type}}}}}'
        if prop_idx < num_props - 1:
            header_data += ',\n'

    end_header_data = \
        '\n};\n\n' \
        '} // namespace detail\n' \
        '} // namespace auc\n'

    return f'{start_header_data}{header_data}{end_header_data}'


cpp_header = get_cpp_unordermap_data(get_properties())
with open('../src/graphemebreakproperty_lookup.hpp', 'w') as gen_header_file:
    gen_header_file.write(cpp_header)
