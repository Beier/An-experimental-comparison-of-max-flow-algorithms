/* 
   Copyright (c) Marshall Clow 2011-2012.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
   
   Thanks to Nevin for his comments/help.
*/

/*
    General problem - turn a sequence of integral types into a sequence of hexadecimal characters.
    - and back.

TO DO:
    1. these should really only work on integral types. (see the >> and << operations)
        -- this is done, I think.
    2. The 'value_type_or_char' struct is really a hack.
        -- but it's a better hack now that it works with back_insert_iterators
*/

/// \file  hex.hpp
/// \brief Convert sequence of integral types into a sequence of hexadecimal 
///     characters and back. Based on the MySQL functions HEX and UNHEX
/// \author Marshall Clow

#ifndef BOOST_ALGORITHM_HEXHPP
#define BOOST_ALGORITHM_HEXHPP

#include <iterator>     // for std::iterator_traits
#include <stdexcept>

#include "../range/begin.hpp"
#include "../range/end.hpp"
#include "../exception/all.hpp"

#include "../utility/enable_if.hpp"
#include "../type_traits/is_integral.hpp"


namespace boost { namespace algorithm {

/*! 
    \struct hex_decode_error 
    \brief  Base exception class for all hex decoding errors 
    
    \struct non_hex_input    
    \brief  Thrown when a non-hex value (0-9, A-F) encountered when decoding.
                Contains the offending character
    
    \struct not_enough_input 
    \brief  Thrown when the input sequence unexpectedly ends
    
*/
struct hex_decode_error : virtual boost::exception, virtual std::exception {};
struct not_enough_input : virtual hex_decode_error {};
struct non_hex_input    : virtual hex_decode_error {};
typedef boost::error_info<struct bad_char_,char> bad_char;

namespace detail {
/// \cond DOXYGEN_HIDE

    template <typename T, typename OutputIterator>
    OutputIterator encode_one ( T val, OutputIterator out ) {
        const std::size_t num_hex_digits =  2 * sizeof ( T );
        char res [ num_hex_digits ];
        char  *p = res + num_hex_digits;
        for ( std::size_t i = 0; i < num_hex_digits; ++i, val >>= 4 )
            *--p = "0123456789ABCDEF" [ val & 0x0F ];
        return std::copy ( res, res + num_hex_digits, out );
        }

// this needs to be in an un-named namespace because it is not a template
// and might get included in several compilation units. This could cause
// multiple definition errors at link time.
    namespace {
    unsigned hex_char_to_int ( char c ) {
        if ( c >= '0' && c <= '9' ) return c - '0';
        if ( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
        if ( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
        BOOST_THROW_EXCEPTION (non_hex_input() << bad_char (c));
        return 0;     // keep dumb compilers happy
        }
    }    

//  My own iterator_traits class.
//  It is here so that I can "reach inside" some kinds of output iterators
//      and get the type to write.
    template <typename Iterator>
    struct hex_iterator_traits {
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
    };

    template<typename Container>
    struct hex_iterator_traits< std::back_insert_iterator<Container> > {
        typedef typename Container::value_type value_type;
    };

    template<typename Container>
    struct hex_iterator_traits< std::front_insert_iterator<Container> > {
        typedef typename Container::value_type value_type;
    };

    template<typename Container>
    struct hex_iterator_traits< std::insert_iterator<Container> > {
        typedef typename Container::value_type value_type;
    };

//  ostream_iterators have three template parameters.
//  The first one is the output type, the second one is the character type of
//  the underlying stream, the third is the character traits.
//      We only care about the first one.
    template<typename T, typename charType, typename traits>
    struct hex_iterator_traits< std::ostream_iterator<T, charType, traits> > {
        typedef T value_type;
    };

    template <typename Iterator> 
    bool iter_end ( Iterator current, Iterator last ) { return current == last; }
  
    template <typename T>
    bool ptr_end ( const T* ptr, const T* /*end*/ ) { return *ptr == '\0'; }
  
//  What can we assume here about the inputs?
//      is std::iterator_traits<InputIterator>::value_type always 'char' ?
//  Could it be wchar_t, say? Does it matter?
//      We are assuming ASCII for the values - but what about the storage?
    template <typename InputIterator, typename OutputIterator, typename EndPred>
    typename boost::enable_if<boost::is_integral<typename hex_iterator_traits<OutputIterator>::value_type>, OutputIterator>::type
    decode_one ( InputIterator &first, InputIterator last, OutputIterator out, EndPred pred ) {
        typedef typename hex_iterator_traits<OutputIterator>::value_type T;
        T res (0);

    //  Need to make sure that we get can read that many chars here.
        for ( std::size_t i = 0; i < 2 * sizeof ( T ); ++i, ++first ) {
            if ( pred ( first, last )) 
                BOOST_THROW_EXCEPTION (not_enough_input ());
            res = ( 16 * res ) + hex_char_to_int (static_cast<char> (*first));
            }
        
        *out = res;
        return ++out;
        }
/// \endcond
    }


/// \fn hex ( InputIterator first, InputIterator last, OutputIterator out )
/// \brief   Converts a sequence of integral types into a hexadecimal sequence of characters.
/// 
/// \param first    The start of the input sequence
/// \param last     One past the end of the input sequence
/// \param out      An output iterator to the results into
/// \return         The updated output iterator
/// \note           Based on the MySQL function of the same name
template <typename InputIterator, typename OutputIterator>
typename boost::enable_if<boost::is_integral<typename detail::hex_iterator_traits<InputIterator>::value_type>, OutputIterator>::type
hex ( InputIterator first, InputIterator last, OutputIterator out ) {
    for ( ; first != last; ++first )
        out = detail::encode_one ( *first, out );
    return out;
    }
    

/// \fn hex ( const T *ptr, OutputIterator out )
/// \brief   Converts a sequence of integral types into a hexadecimal sequence of characters.
/// 
/// \param ptr      A pointer to a 0-terminated sequence of data.
/// \param out      An output iterator to the results into
/// \return         The updated output iterator
/// \note           Based on the MySQL function of the same name
template <typename T, typename OutputIterator>
typename boost::enable_if<boost::is_integral<T>, OutputIterator>::type
hex ( const T *ptr, OutputIterator out ) {
    while ( *ptr )
        out = detail::encode_one ( *ptr++, out );
    return out;
    }

/// \fn hex ( const Range &r, OutputIterator out )
/// \brief   Converts a sequence of integral types into a hexadecimal sequence of characters.
/// 
/// \param r        The input range
/// \param out      An output iterator to the results into
/// \return         The updated output iterator
/// \note           Based on the MySQL function of the same name
template <typename Range, typename OutputIterator>
typename boost::enable_if<boost::is_integral<typename detail::hex_iterator_traits<typename Range::iterator>::value_type>, OutputIterator>::type
hex ( const Range &r, OutputIterator out ) {
    return hex (boost::begin(r), boost::end(r), out);
}


/// \fn unhex ( InputIterator first, InputIterator last, OutputIterator out )
/// \brief   Converts a sequence of hexadecimal characters into a sequence of integers.
/// 
/// \param first    The start of the input sequence
/// \param last     One past the end of the input sequence
/// \param out      An output iterator to the results into
/// \return         The updated output iterator
/// \note           Based on the MySQL function of the same name
template <typename InputIterator, typename OutputIterator>
OutputIterator unhex ( InputIterator first, InputIterator last, OutputIterator out ) {
    while ( first != last )
        out = detail::decode_one ( first, last, out, detail::iter_end<InputIterator> );
    return out;
    }


/// \fn unhex ( const T *ptr, OutputIterator out )
/// \brief   Converts a sequence of hexadecimal characters into a sequence of integers.
/// 
/// \param ptr      A pointer to a null-terminated input sequence.
/// \param out      An output iterator to the results into
/// \return         The updated output iterator
/// \note           Based on the MySQL function of the same name
template <typename T, typename OutputIterator>
OutputIterator unhex ( const T *ptr, OutputIterator out ) {
    typedef typename detail::hex_iterator_traits<OutputIterator>::value_type OutputType;
//  If we run into the terminator while decoding, we will throw a
//      malformed input exception. It would be nicer to throw a 'Not enough input'
//      exception - but how much extra work would that require?
    while ( *ptr )
        out = detail::decode_one ( ptr, (const T *) NULL, out, detail::ptr_end<T> );
    return out;
    }


/// \fn OutputIterator unhex ( const Range &r, OutputIterator out )
/// \brief   Converts a sequence of hexadecimal characters into a sequence of integers.
/// 
/// \param r        The input range
/// \param out      An output iterator to the results into
/// \return         The updated output iterator
/// \note           Based on the MySQL function of the same name
template <typename Range, typename OutputIterator>
OutputIterator unhex ( const Range &r, OutputIterator out ) {
    return unhex (boost::begin(r), boost::end(r), out);
    }


/// \fn String hex ( const String &input )
/// \brief   Converts a sequence of integral types into a hexadecimal sequence of characters.
/// 
/// \param input    A container to be converted
/// \return         A container with the encoded text
template<typename String>
String hex ( const String &input ) {
    String output;
    output.reserve (input.size () * (2 * sizeof (typename String::value_type)));
    (void) hex (input, std::back_inserter (output));
    return output;
    }

/// \fn String unhex ( const String &input )
/// \brief   Converts a sequence of hexadecimal characters into a sequence of characters.
/// 
/// \param input    A container to be converted
/// \return         A container with the decoded text
template<typename String>
String unhex ( const String &input ) {
    String output;
    output.reserve (input.size () / (2 * sizeof (typename String::value_type)));
    (void) unhex (input, std::back_inserter (output));
    return output;
    }

}}

#endif // BOOST_ALGORITHM_HEXHPP
