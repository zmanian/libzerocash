/** @file
 *****************************************************************************

 Test for Merkle tree.

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include "libzerocash/IncrementalMerkleTree.h"

#include <iostream>
#include <vector>

#define BOOST_TEST_MODULE merkleTest
#include <boost/test/included/unit_test.hpp>

using namespace libzerocash;
using namespace std;

void constructNonzeroTestVector(std::vector< std::vector<bool> > &values, uint32_t size)
{
    values.resize(0);
    std::vector<bool> dummy;
    dummy.resize(256);
    dummy[0] = true;

    for (uint32_t i = 0; i < size; i++)
    {
        values.push_back(dummy);
    }
}

void constructZeroTestVector(std::vector< std::vector<bool> > &values, uint32_t size)
{
    values.resize(0);
    std::vector<bool> dummy;
    dummy.resize(256);

    for (uint32_t i = 0; i < size; i++)
    {
        values.push_back(dummy);
    }
}

size_t countOnes(std::vector<bool> bits)
{
    size_t count = 0;
    for (size_t i = 0; i < bits.size(); i++) {
        if (bits.at(i)) {
            count++;
        }
    }
    return count;
}

BOOST_AUTO_TEST_CASE( testRootOfTreeOfZerosIsZero ) {
    IncrementalMerkleTree incTree;
    std::vector< std::vector<bool> > values;
    std::vector<bool> actual_root;

    constructZeroTestVector(values, 2);

    // Create an IncrementalMerkleTree over the values.
    if (incTree.insertVector(values) == false) {
        BOOST_ERROR("Could not insert into the tree.");
    }
    incTree.prune();
    incTree.getRootValue(actual_root);

    std::vector<bool> expected_root(32*8, 0);
    BOOST_CHECK( expected_root == actual_root );
}

BOOST_AUTO_TEST_CASE( testRootOfTreeOfNonZeroIsNonZero ) {
    IncrementalMerkleTree incTree;
    std::vector< std::vector<bool> > values;
    std::vector<bool> actual_root;

    constructNonzeroTestVector(values, 2);

    // Create an IncrementalMerkleTree over the values.
    if (incTree.insertVector(values) == false) {
        BOOST_ERROR("Could not insert into the tree.");
    }
    incTree.prune();
    incTree.getRootValue(actual_root);

    std::vector<bool> expected_root(32*8, 0);
    BOOST_CHECK( expected_root != actual_root );
}

BOOST_AUTO_TEST_CASE( testCompactRepresentation ) {
    for (uint32_t num_entries = 0; num_entries < 100; num_entries++) {
        std::vector< std::vector<bool> > values;
        std::vector<bool> root1, root2;
        IncrementalMerkleTree incTree(64);

        constructNonzeroTestVector(values, num_entries);

        BOOST_REQUIRE( incTree.insertVector(values) );
        BOOST_REQUIRE( incTree.prune() );

        IncrementalMerkleTreeCompact compact = incTree.getCompactRepresentation();

        BOOST_REQUIRE( compact.getTreeHeight() == 64 );

        // Calculate what the path to the next-added element should be.
        std::vector<unsigned char> path_bytes(8);
        std::vector<bool> path_bits;
        libzerocash::convertIntToBytesVector(num_entries, path_bytes);
        libzerocash::convertBytesVectorToVector(path_bytes, path_bits);

        // Make sure the paths match.
        BOOST_REQUIRE( compact.getHashList() == path_bits );
        BOOST_REQUIRE( compact.getHashListBytes() == path_bytes );

        // Make sure there's a hash for every '1' bit down the path.
        BOOST_REQUIRE( compact.getHashVec().size() == countOnes(path_bits) );

        // Make sure 'restoring' the tree results in the same root.
        IncrementalMerkleTree newTree(compact);
        incTree.getRootValue(root1);
        incTree.getRootValue(root2);
        BOOST_REQUIRE( root1 == root2 );
    }
}
