#ifndef SIMULATED_SIMIAN_GENERATE_H_INCLUDED
#define SIMULATED_SIMIAN_GENERATE_H_INCLUDED

#include <iterator>

template <typename output_expr_t, typename range_t, typename predictate_t>
struct generate_impl
{
    output_expr_t m_output_expr;
    range_t m_range;
    predictate_t m_predictate;

    typedef typename range_t::value_type value_t;

    generate_impl (output_expr_t output_expr, range_t range, predictate_t predictate) :
        m_output_expr(output_expr), m_range(range), m_predictate(predictate)
    {
    }

    struct iterator
    {
        typedef std::input_iterator_tag iterator_category;
        typedef value_t value_type;
        typedef value_t difference_type;
        typedef value_t* pointer;
        typedef value_t& reference;

        generate_impl& gen;
        typename range_t::iterator itr;

        value_t operator *()
        {
            return gen.m_output_expr (*itr);
        }

        iterator& operator++()
        {
            while (++itr != gen.m_range.end() && !gen.m_predictate(*itr))
                ;
            return *this;
        }

        bool operator != (const iterator& that)
        {
            return itr != that.itr;
        }

        bool operator == (const iterator& that)
        {
            return itr == that.itr;
        }
    };

    iterator begin ()
    {
        auto itr = m_range.begin();
        while (itr != m_range.end() && !m_predictate(*itr))
            ++itr;

        return iterator{*this, itr};
    }

    iterator end ()
    {
        return iterator{*this, m_range.end()};
    }


    template <typename collection_t>
    operator collection_t ()
    {
        collection_t c;
        for (auto val: m_range)
        {
            if (m_predictate(val))
                c.push_back(m_output_expr(val));
        }
        return c;
    }
    
};

template <typename value_t>
bool true_predictate (const value_t&) 
{
    return true;
}

template <typename output_expr_t, typename range_t, typename predictate_t>
auto generate (output_expr_t expr, const range_t& r, predictate_t p) 
                   -> generate_impl<output_expr_t, range_t, predictate_t>
{
    return generate_impl<output_expr_t, range_t, predictate_t> (expr, r, p);
}

template <typename output_expr_t, typename range_t>
auto generate (output_expr_t expr, const range_t& r) 
                   -> generate_impl<output_expr_t, range_t, decltype (&true_predictate<typename range_t::value_type>)>
{
    return generate(expr, r, true_predictate<typename range_t::value_type>);
}

#endif
