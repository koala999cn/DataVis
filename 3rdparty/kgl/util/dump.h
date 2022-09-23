#include <ostream>
#include "../base/traits_helper.h"


// dump graph "g" to file "path"
template<typename GRAPH>
void dump(const GRAPH& g, std::ostream& os)
{
    auto V = static_cast<unsigned>(g.order());
    for (unsigned i = 0; i < V; i++) {
        unsigned jMin = g.isDigraph() ? 0 : i;
        for (unsigned j = jMin; j < V; j++) {
            auto r = g.edges(i, j);
            if (r.empty())
                continue;

            os << "(" << i << ", " << j;
            for (; !r.empty(); ++r) {
                os << ", ";
                if constexpr (has_operator_output<decltype(os), decltype(r)>::value)
                    os << *r;
                else
                    os << "?"; // Êä³öÕ¼Î»·ûºÅ?
            }
                    
            os << ") ";
        }

        os << "\n";
    }
}