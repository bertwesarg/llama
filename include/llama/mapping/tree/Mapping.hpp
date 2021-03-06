/* Copyright 2018 Alexander Matthes
 *
 * This file is part of LLAMA.
 *
 * LLAMA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * LLAMA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with LLAMA.  If not, see <www.gnu.org/licenses/>.
 */

#pragma once

#include "../../Types.hpp"
#include "../../UserDomain.hpp"

#include "TreeElement.hpp"
#include "TreeFromDomains.hpp"
#include "GetBlobSize.hpp"
#include "GetBlobByte.hpp"

#include "toString.hpp"

#include "functor/Idem.hpp"
#include "functor/MoveRTDown.hpp"
#include "functor/LeafOnlyRT.hpp"

#include "MergeFunctors.hpp"

#include <type_traits>

namespace llama
{

namespace mapping
{

namespace tree
{

/** Free describable mapping which can be used for creating a \ref View with a
 *  \ref Factory. For the interface details see \ref Factory.
 * \tparam T_UserDomain type of the user domain
 * \tparam T_DatumDomain type of the datum domain
 * \tparam T_TreeOperationList the type of a compile time list (\ref Tuple) used
 *  to define the tree mapping
 */
template<
    typename T_UserDomain,
    typename T_DatumDomain,
    typename T_TreeOperationList
>
struct Mapping
{
    using UserDomain = T_UserDomain;
    using DatumDomain = T_DatumDomain;
    using BasicTree = TreeFromDomains<
        UserDomain,
        DatumDomain
    >;
    //TODO, support more than one blob
    static constexpr std::size_t blobCount = 1;

    using MergedFunctors = MergeFunctors<
        BasicTree,
        T_TreeOperationList
    >;

    using ResultTree = typename MergedFunctors::Result;

    UserDomain const userDomainSize;
    BasicTree const basicTree;
    MergedFunctors const mergedFunctors;
    ResultTree const resultTree;

    /** The initalization of this mapping needs a \ref Tuple of operations
     *  which describe the mapping in detail. Please have a look at the user
     *  documenation for more information.
     * \param size the size of the user domain
     * \param treeOperationList list of operations to define the mapping, e.g.
     *  \ref functor::Idem, \ref functor::LeafOnlyRT, \ref functor::MoveRTDown.
     */
    LLAMA_FN_HOST_ACC_INLINE
    Mapping(
        UserDomain const size,
        T_TreeOperationList const treeOperationList
    ) :
        userDomainSize( size ),
        basicTree( setUserDomainInTree< DatumDomain >( size ) ),
        mergedFunctors(
            basicTree,
            treeOperationList
        ),
        resultTree( mergedFunctors.basicToResult( basicTree ) )
    { }

    Mapping() = default;
    Mapping( Mapping const & ) = default;
    Mapping( Mapping && ) = default;
    ~Mapping( ) = default;

    LLAMA_FN_HOST_ACC_INLINE
    auto
    getBlobSize( std::size_t const ) const
    -> std::size_t
    {
        return getTreeBlobSize( resultTree );
    }

    template< std::size_t... T_datumDomainCoord >
    LLAMA_FN_HOST_ACC_INLINE
    auto
    getBlobByte( UserDomain const coord ) const
    -> std::size_t
    {
        auto const basicTreeCoord = getBasicTreeCoordFromDomains<
            DatumCoord< T_datumDomainCoord... >
        > ( coord );

        auto const resultTreeCoord = mergedFunctors.basicCoordToResultCoord(
            basicTreeCoord,
            basicTree
        );

        return getTreeBlobByte(
            resultTree,
            resultTreeCoord
        );
    }

    template< std::size_t... T_datumDomainCoord >
    LLAMA_FN_HOST_ACC_INLINE
    constexpr
    auto
    getBlobNr( UserDomain const coord ) const
    -> std::size_t
    {
        return 0;
    }
};

} // namespace tree

} // namespace mapping

} // namespace llama
