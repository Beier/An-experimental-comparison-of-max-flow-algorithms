/*=============================================================================
    Copyright (c) 2012 Nathan Ridge

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_ADAPTED_STRUCT_DETAIL_DEFINE_STRUCT_INLINE_HPP
#define BOOST_FUSION_ADAPTED_STRUCT_DETAIL_DEFINE_STRUCT_INLINE_HPP

#include "../../../../fusion/support/category_of.hpp"
#include "../../../../fusion/sequence/sequence_facade.hpp"
#include "../../../../fusion/iterator/iterator_facade.hpp"
#include "../../../../fusion/algorithm/auxiliary/copy.hpp"
#include "../../../../fusion/adapted/struct/detail/define_struct.hpp"
#include "../../../../mpl/int.hpp"
#include "../../../../mpl/bool.hpp"
#include "../../../../mpl/identity.hpp"
#include "../../../../mpl/minus.hpp"
#include "../../../../mpl/if.hpp"
#include "../../../../type_traits/is_const.hpp"
#include "../../../../preprocessor/comma_if.hpp"
#include "../../../../preprocessor/facilities/is_empty.hpp"
#include "../../../../preprocessor/repeat.hpp"
#include "../../../../preprocessor/seq/for_each_i.hpp"
#include "../../../../preprocessor/seq/size.hpp"
#include "../../../../preprocessor/seq/enum.hpp"
#include "../../../../preprocessor/tuple/elem.hpp"

#define BOOST_FUSION_MAKE_DEFAULT_INIT_LIST_ENTRY(R, DATA, N, ATTRIBUTE)        \
    BOOST_PP_COMMA_IF(N) BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE)()

#define BOOST_FUSION_MAKE_DEFAULT_INIT_LIST(ATTRIBUTES_SEQ)                     \
            : BOOST_PP_SEQ_FOR_EACH_I(                                          \
              BOOST_FUSION_MAKE_DEFAULT_INIT_LIST_ENTRY,                        \
              ~,                                                                \
              ATTRIBUTES_SEQ)                                                   \

#define BOOST_FUSION_IGNORE_1(ARG1)
#define BOOST_FUSION_IGNORE_2(ARG1, ARG2)

#define BOOST_FUSION_MAKE_COPY_CONSTRUCTOR(NAME, ATTRIBUTES_SEQ)                \
    NAME(BOOST_PP_SEQ_FOR_EACH_I(                                               \
            BOOST_FUSION_MAKE_CONST_REF_PARAM,                                  \
            ~,                                                                  \
            ATTRIBUTES_SEQ))                                                    \
        : BOOST_PP_SEQ_FOR_EACH_I(                                              \
              BOOST_FUSION_MAKE_INIT_LIST_ENTRY,                                \
              ~,                                                                \
              ATTRIBUTES_SEQ)                                                   \
    {                                                                           \
    }                                                                           \

#define BOOST_FUSION_MAKE_CONST_REF_PARAM(R, DATA, N, ATTRIBUTE)                \
    BOOST_PP_COMMA_IF(N)                                                        \
    BOOST_PP_TUPLE_ELEM(2, 0, ATTRIBUTE) const&                                 \
    BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE)

#define BOOST_FUSION_MAKE_INIT_LIST_ENTRY_I(NAME) NAME(NAME)

#define BOOST_FUSION_MAKE_INIT_LIST_ENTRY(R, DATA, N, ATTRIBUTE)                \
    BOOST_PP_COMMA_IF(N)                                                        \
    BOOST_FUSION_MAKE_INIT_LIST_ENTRY_I(BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE))

#define BOOST_FUSION_ITERATOR_NAME(NAME)                                        \
    BOOST_PP_CAT(boost_fusion_detail_, BOOST_PP_CAT(NAME, _iterator))

// Note: all template parameter names need to be uglified, otherwise they might
//       shadow a template parameter of the struct when used with
//       BOOST_FUSION_DEFINE_TPL_STRUCT_INLINE

#define BOOST_FUSION_MAKE_ITERATOR_VALUE_OF_SPECS(Z, N, NAME)                   \
    template <typename boost_fusion_detail_Sq>                                  \
    struct value_of<                                                            \
               BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Sq, N>      \
           >                                                                    \
        : boost::mpl::identity<                                                 \
              typename boost_fusion_detail_Sq::t##N##_type                      \
          >                                                                     \
    {                                                                           \
    };

#define BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                  \
    SPEC_TYPE, CALL_ARG_TYPE, TYPE_QUAL, ATTRIBUTE, N)                          \
                                                                                \
    template <typename boost_fusion_detail_Sq>                                  \
    struct deref<SPEC_TYPE, N> >                                                \
    {                                                                           \
        typedef typename boost_fusion_detail_Sq::t##N##_type TYPE_QUAL& type;   \
        static type call(CALL_ARG_TYPE, N> const& iter)                         \
        {                                                                       \
            return iter.seq_.BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE);              \
        }                                                                       \
    };

#define BOOST_FUSION_MAKE_ITERATOR_DEREF_SPECS(R, NAME, N, ATTRIBUTE)           \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Sq,                \
        BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Sq,                \
        ,                                                                       \
        ATTRIBUTE,                                                              \
        N)                                                                      \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        BOOST_FUSION_ITERATOR_NAME(NAME)<const boost_fusion_detail_Sq,          \
        BOOST_FUSION_ITERATOR_NAME(NAME)<const boost_fusion_detail_Sq,          \
        const,                                                                  \
        ATTRIBUTE,                                                              \
        N)                                                                      \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        const BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Sq,          \
        BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Sq,                \
        ,                                                                       \
        ATTRIBUTE,                                                              \
        N)                                                                      \
    BOOST_FUSION_MAKE_ITERATOR_DEREF_SPEC(                                      \
        const BOOST_FUSION_ITERATOR_NAME(NAME)<const boost_fusion_detail_Sq,    \
        BOOST_FUSION_ITERATOR_NAME(NAME)<const boost_fusion_detail_Sq,          \
        const,                                                                  \
        ATTRIBUTE,                                                              \
        N)                                                                      \

#define BOOST_FUSION_MAKE_VALUE_AT_SPECS(Z, N, DATA)                            \
    template <typename boost_fusion_detail_Sq>                                  \
    struct value_at<boost_fusion_detail_Sq, boost::mpl::int_<N> >               \
    {                                                                           \
        typedef typename boost_fusion_detail_Sq::t##N##_type type;              \
    };

#define BOOST_FUSION_MAKE_AT_SPECS(R, DATA, N, ATTRIBUTE)                       \
    template <typename boost_fusion_detail_Sq>                                  \
    struct at<boost_fusion_detail_Sq, boost::mpl::int_<N> >                     \
    {                                                                           \
        typedef typename boost::mpl::if_<                                       \
            boost::is_const<boost_fusion_detail_Sq>,                            \
            typename boost_fusion_detail_Sq::t##N##_type const&,                \
            typename boost_fusion_detail_Sq::t##N##_type&                       \
        >::type type;                                                           \
                                                                                \
        static type call(boost_fusion_detail_Sq& sq)                            \
        {                                                                       \
            return sq. BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE);                    \
        }                                                                       \
    };

#define BOOST_FUSION_MAKE_TYPEDEF(R, DATA, N, ATTRIBUTE)                        \
    typedef BOOST_PP_TUPLE_ELEM(2, 0, ATTRIBUTE) t##N##_type;

#define BOOST_FUSION_MAKE_DATA_MEMBER(R, DATA, N, ATTRIBUTE)                    \
    BOOST_PP_TUPLE_ELEM(2, 0, ATTRIBUTE) BOOST_PP_TUPLE_ELEM(2, 1, ATTRIBUTE);

// Note: We can't nest the iterator inside the struct because we run into
//       a MSVC10 bug involving partial specializations of nested templates.

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_IMPL(NAME, ATTRIBUTES)                \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR(NAME, ATTRIBUTES)                \
    struct NAME : boost::fusion::sequence_facade<                               \
                      NAME,                                                     \
                      boost::fusion::random_access_traversal_tag                \
                  >                                                             \
    {                                                                           \
        BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS(NAME, ATTRIBUTES)             \
    };

#define BOOST_FUSION_DEFINE_TPL_STRUCT_INLINE_IMPL(                             \
    TEMPLATE_PARAMS_SEQ, NAME, ATTRIBUTES)                                      \
                                                                                \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR(NAME, ATTRIBUTES)                \
                                                                                \
    template <                                                                  \
        BOOST_FUSION_ADAPT_STRUCT_UNPACK_TEMPLATE_PARAMS_IMPL(                  \
            (0)TEMPLATE_PARAMS_SEQ)                                             \
    >                                                                           \
    struct NAME : boost::fusion::sequence_facade<                               \
                      NAME<                                                     \
                          BOOST_PP_SEQ_ENUM(TEMPLATE_PARAMS_SEQ)                \
                      >,                                                        \
                      boost::fusion::random_access_traversal_tag                \
                  >                                                             \
    {                                                                           \
        BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS(NAME, ATTRIBUTES)             \
    };

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS(NAME, ATTRIBUTES)             \
    BOOST_FUSION_DEFINE_STRUCT_MEMBERS_IMPL(                                    \
        NAME,                                                                   \
        BOOST_PP_CAT(BOOST_FUSION_ADAPT_STRUCT_FILLER_0 ATTRIBUTES,_END))

// Note: can't compute BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ) directly because
//       ATTRIBUTES_SEQ may be empty and calling BOOST_PP_SEQ_SIZE on an empty
//       sequence produces warnings on MSVC.
#define BOOST_FUSION_DEFINE_STRUCT_MEMBERS_IMPL(NAME, ATTRIBUTES_SEQ)           \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS_IMPL_IMPL(                        \
        NAME,                                                                   \
        ATTRIBUTES_SEQ,                                                         \
        BOOST_PP_DEC(BOOST_PP_SEQ_SIZE((0)ATTRIBUTES_SEQ)))

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR(NAME, ATTRIBUTES)            \
    BOOST_FUSION_DEFINE_STRUCT_ITERATOR_IMPL(                                   \
        NAME,                                                                   \
        BOOST_PP_CAT(BOOST_FUSION_ADAPT_STRUCT_FILLER_0 ATTRIBUTES,_END))

#define BOOST_FUSION_DEFINE_STRUCT_ITERATOR_IMPL(NAME, ATTRIBUTES_SEQ)          \
    BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR_IMPL_IMPL(                       \
        NAME,                                                                   \
        ATTRIBUTES_SEQ,                                                         \
        BOOST_PP_DEC(BOOST_PP_SEQ_SIZE((0)ATTRIBUTES_SEQ)))

#define BOOST_FUSION_DEFINE_STRUCT_INLINE_ITERATOR_IMPL_IMPL(                   \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTES_SEQ_SIZE)                                  \
                                                                                \
    template <typename boost_fusion_detail_Seq, int N>                          \
    struct BOOST_FUSION_ITERATOR_NAME(NAME)                                     \
        : boost::fusion::iterator_facade<                                       \
              BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Seq, N>,     \
              boost::fusion::random_access_traversal_tag                        \
          >                                                                     \
    {                                                                           \
        typedef boost::mpl::int_<N> index;                                      \
        typedef boost_fusion_detail_Seq sequence_type;                          \
                                                                                \
        BOOST_FUSION_ITERATOR_NAME(NAME)(boost_fusion_detail_Seq& seq)          \
            : seq_(seq) {}                                                      \
                                                                                \
        boost_fusion_detail_Seq& seq_;                                          \
                                                                                \
        template <typename boost_fusion_detail_T> struct value_of;              \
        BOOST_PP_REPEAT(                                                        \
            ATTRIBUTES_SEQ_SIZE,                                                \
            BOOST_FUSION_MAKE_ITERATOR_VALUE_OF_SPECS,                          \
            NAME)                                                               \
                                                                                \
        template <typename boost_fusion_detail_T> struct deref;                 \
        BOOST_PP_SEQ_FOR_EACH_I(                                                \
            BOOST_FUSION_MAKE_ITERATOR_DEREF_SPECS,                             \
            NAME,                                                               \
            ATTRIBUTES_SEQ)                                                     \
                                                                                \
        template <typename boost_fusion_detail_It>                              \
        struct next                                                             \
        {                                                                       \
            typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                           \
                typename boost_fusion_detail_It::sequence_type,                 \
                boost_fusion_detail_It::index::value + 1                        \
            > type;                                                             \
                                                                                \
            static type call(boost_fusion_detail_It const& it)                  \
            {                                                                   \
                return type(it.seq_);                                           \
            }                                                                   \
        };                                                                      \
                                                                                \
        template <typename boost_fusion_detail_It>                              \
        struct prior                                                            \
        {                                                                       \
            typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                           \
                typename boost_fusion_detail_It::sequence_type,                 \
                boost_fusion_detail_It::index::value - 1                        \
            > type;                                                             \
                                                                                \
            static type call(boost_fusion_detail_It const& it)                  \
            {                                                                   \
                return type(it.seq_);                                           \
            }                                                                   \
        };                                                                      \
                                                                                \
        template <                                                              \
            typename boost_fusion_detail_It1,                                   \
            typename boost_fusion_detail_It2                                    \
        >                                                                       \
        struct distance                                                         \
        {                                                                       \
            typedef typename boost::mpl::minus<                                 \
                typename boost_fusion_detail_It2::index,                        \
                typename boost_fusion_detail_It1::index                         \
            >::type type;                                                       \
                                                                                \
             static type call(boost_fusion_detail_It1 const& it1,               \
                              boost_fusion_detail_It2 const& it2)               \
            {                                                                   \
                return type();                                                  \
            }                                                                   \
        };                                                                      \
                                                                                \
        template <                                                              \
            typename boost_fusion_detail_It,                                    \
            typename boost_fusion_detail_M                                      \
        >                                                                       \
        struct advance                                                          \
        {                                                                       \
            typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                           \
                typename boost_fusion_detail_It::sequence_type,                 \
                boost_fusion_detail_It::index::value                            \
                    + boost_fusion_detail_M::value                              \
            > type;                                                             \
                                                                                \
            static type call(boost_fusion_detail_It const& it)                  \
            {                                                                   \
                return type(it.seq_);                                           \
            }                                                                   \
        };                                                                      \
    };


#define BOOST_FUSION_DEFINE_STRUCT_INLINE_MEMBERS_IMPL_IMPL(                    \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTES_SEQ_SIZE)                                  \
                                                                                \
    /* Note: second BOOST_PP_IF is necessary to avoid MSVC warning when */      \
    /*       calling BOOST_FUSION_IGNORE_1 with no arguments.           */      \
    NAME()                                                                      \
        BOOST_PP_IF(                                                            \
            ATTRIBUTES_SEQ_SIZE,                                                \
            BOOST_FUSION_MAKE_DEFAULT_INIT_LIST,                                \
            BOOST_FUSION_IGNORE_1)                                              \
                (BOOST_PP_IF(                                                   \
                    ATTRIBUTES_SEQ_SIZE,                                        \
                    ATTRIBUTES_SEQ,                                             \
                    0))                                                         \
    {                                                                           \
    }                                                                           \
                                                                                \
    BOOST_PP_IF(                                                                \
        ATTRIBUTES_SEQ_SIZE,                                                    \
        BOOST_FUSION_MAKE_COPY_CONSTRUCTOR,                                     \
        BOOST_FUSION_IGNORE_2)                                                  \
            (NAME, ATTRIBUTES_SEQ)                                              \
                                                                                \
    template <typename boost_fusion_detail_Seq>                                 \
    NAME(const boost_fusion_detail_Seq& rhs)                                    \
    {                                                                           \
        boost::fusion::copy(rhs, *this);                                        \
    }                                                                           \
                                                                                \
    template <typename boost_fusion_detail_Seq>                                 \
    NAME& operator=(const boost_fusion_detail_Seq& rhs)                         \
    {                                                                           \
        boost::fusion::copy(rhs, *this);                                        \
        return *this;                                                           \
    }                                                                           \
                                                                                \
    template <typename boost_fusion_detail_Sq>                                  \
    struct begin                                                                \
    {                                                                           \
        typedef BOOST_FUSION_ITERATOR_NAME(NAME)<boost_fusion_detail_Sq, 0>     \
             type;                                                              \
                                                                                \
        static type call(boost_fusion_detail_Sq& sq)                            \
        {                                                                       \
            return type(sq);                                                    \
        }                                                                       \
    };                                                                          \
                                                                                \
    template <typename boost_fusion_detail_Sq>                                  \
    struct end                                                                  \
    {                                                                           \
        typedef BOOST_FUSION_ITERATOR_NAME(NAME)<                               \
            boost_fusion_detail_Sq,                                             \
            ATTRIBUTES_SEQ_SIZE                                                 \
        > type;                                                                 \
                                                                                \
        static type call(boost_fusion_detail_Sq& sq)                            \
        {                                                                       \
            return type(sq);                                                    \
        }                                                                       \
    };                                                                          \
                                                                                \
    template <typename boost_fusion_detail_Sq>                                  \
    struct size : boost::mpl::int_<ATTRIBUTES_SEQ_SIZE>                         \
    {                                                                           \
    };                                                                          \
                                                                                \
    template <typename boost_fusion_detail_Sq>                                  \
    struct empty : boost::mpl::bool_<ATTRIBUTES_SEQ_SIZE == 0>                  \
    {                                                                           \
    };                                                                          \
                                                                                \
    template <                                                                  \
        typename boost_fusion_detail_Sq,                                        \
        typename boost_fusion_detail_N                                          \
    >                                                                           \
    struct value_at : value_at<                                                 \
                          boost_fusion_detail_Sq,                               \
                          boost::mpl::int_<boost_fusion_detail_N::value>        \
                      >                                                         \
    {                                                                           \
    };                                                                          \
                                                                                \
    BOOST_PP_REPEAT(                                                            \
        ATTRIBUTES_SEQ_SIZE,                                                    \
        BOOST_FUSION_MAKE_VALUE_AT_SPECS,                                       \
        ~)                                                                      \
                                                                                \
    template <                                                                  \
        typename boost_fusion_detail_Sq,                                        \
        typename boost_fusion_detail_N                                          \
    >                                                                           \
    struct at : at<                                                             \
                    boost_fusion_detail_Sq,                                     \
                    boost::mpl::int_<boost_fusion_detail_N::value>              \
                >                                                               \
    {                                                                           \
    };                                                                          \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(BOOST_FUSION_MAKE_AT_SPECS, ~, ATTRIBUTES_SEQ)      \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(BOOST_FUSION_MAKE_TYPEDEF, ~, ATTRIBUTES_SEQ)       \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(                                                    \
        BOOST_FUSION_MAKE_DATA_MEMBER,                                          \
        ~,                                                                      \
        ATTRIBUTES_SEQ)

#endif
