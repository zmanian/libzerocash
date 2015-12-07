/** @file
 *****************************************************************************

 Declaration of interfaces for the classes Address and PublicAddress.

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <vector>
#include <string>

#include "serialize.h"

namespace libzerocash {

/***************************** Private address ********************************/

class PrivateAddress {
public:
    PrivateAddress(const std::vector<unsigned char> a_sk, const std::string sk_enc);

    bool operator==(const PrivateAddress& rhs) const;
    bool operator!=(const PrivateAddress& rhs) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(a_sk);
        READWRITE(sk_enc);
    }

    const std::vector<unsigned char>& getAddressSecret() const;
    const std::string getEncryptionSecretKey() const;

private:
    std::vector<unsigned char> a_sk;
    std::string sk_enc;

};

/***************************** Public address ********************************/

class PublicAddress {
public:
    // XXX: HACK: this is only here because of Coin's hacky non-constructor.
    PublicAddress();
    PublicAddress(const PrivateAddress& addr_sk);

    bool operator==(const PublicAddress& rhs) const;
    bool operator!=(const PublicAddress& rhs) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(a_pk);
        READWRITE(pk_enc);
    }

    const std::vector<unsigned char>& getPublicAddressSecret() const;
    const std::string getEncryptionPublicKey() const;

private:
    std::vector<unsigned char> a_pk;
    std::string pk_enc;
};

/******************************** Address ************************************/

class Address {
public:
    Address(PrivateAddress&);

    const PublicAddress& getPublicAddress() const;
    const PrivateAddress& getPrivateAddress() const;

    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(addr_pk);
        READWRITE(addr_sk);
    }

    static Address CreateNewRandomAddress();

private:
    PublicAddress addr_pk;
    PrivateAddress addr_sk;
};

} /* namespace libzerocash */

#endif /* ADDRESS_H_ */
