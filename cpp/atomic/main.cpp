#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <chrono>
#include <stdexcept>
#include <memory>
#include <iostream>

#include "externals.hpp"


class Storelike;

class Value {
    void * value;
public:
    Value(std::string s) {
        value = new std::string(s);
    }
    Value(int64_t i) {
        value = new int64_t(i);
    }
    Value(std::unordered_map<std::string, Value> v) {
        value = new std::unordered_map<std::string, Value>(v);
    }

    std::vector<std::string> to_vec() {
        return *(std::vector<std::string>*)value;
    }

    std::string to_string() {
        return *(std::string*)value;
    }

    template<typename T>
    Value(std::vector<T> v) {
        value = new std::vector<T>(v);
    }
};
class Resource {
public:
    std::unordered_map<std::string, std::shared_ptr<Value>> properties;
    std::string url;

    Resource(const std::string& url) : url(url) {}

    std::optional<Value> get(const std::string& key) {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return *it->second;
        }
        return std::nullopt;
    }

    void set_propval(const std::string& key, Value value, std::shared_ptr<Storelike>& store) {
        properties[key] = std::make_shared<Value>(value);
    }

    void remove_propval(const std::string& key) {
        properties.erase(key);
    }

    void check_required_props(const std::shared_ptr<Storelike>& store) {
        // Implementation of check_required_props
    }

    std::string to_string() {
        return url;
    }
};


class Storelike {
public: 
    virtual std::string get_base_url() = 0;
    virtual std::optional<std::string> get_self_url() = 0;
    virtual void add_resource(const std::shared_ptr<Resource>& resource) = 0;
    virtual void remove_resource(const std::string& url) = 0;
    virtual std::optional<Resource> get_resource(const std::string& url) = 0;
};

bool check_write(
    const std::shared_ptr<Storelike>& store,
    const std::shared_ptr<Resource>& resource,
    const std::string& agent
);

namespace datetime_helpers {
    uint64_t now() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }
}

// Assuming these are defined elsewhere in your C++ codebase
namespace urls {
    const std::string WRITE = "https://atomicdata.dev/properties/write";
    const std::string PARENT = "https://atomicdata.dev/properties/parent";
    const std::string SUBJECT = "https://atomicdata.dev/properties/subject";
    const std::string CREATED_AT = "https://atomicdata.dev/properties/createdAt";
    const std::string SIGNER = "https://atomicdata.dev/properties/signer";
    const std::string SET = "https://atomicdata.dev/properties/SET";
    const std::string REMOVE = "https://atomicdata.dev/properties/remove";
    const std::string DESTROY = "https://atomicdata.dev/properties/destroy";
    const std::string SIGNATURE = "https://atomicdata.dev/properties/signature";
    const std::string PUBLIC_KEY = "https://atomicdata.dev/properties/publicKey";
}

class Commit {
public:
    std::string subject;
    int64_t created_at;
    std::string signer;
    std::optional<std::unordered_map<std::string, Value>> set;
    std::optional<std::vector<std::string>> remove;
    std::optional<bool> destroy;
    std::optional<std::string> signature;
    std::optional<std::string> url;

    std::shared_ptr<Resource> apply_opts(
        std::shared_ptr<Storelike>& store,
        bool validate_schema,
        bool validate_signature,
        bool validate_timestamp,
        bool validate_rights
    ) {
        if (validate_signature) {
            if (!signature) {
                throw std::runtime_error("No signature set");
            }
            
            // Fetch the public key of the signer
            auto signer_resource = store->get_resource(signer);
            std::string pubkey_b64 = signer_resource->get(urls::PUBLIC_KEY)->to_string();
            std::vector<uint8_t> agent_pubkey = base64::decode<std::vector<uint8_t>>(pubkey_b64);

            // Serialize the commit deterministically
            std::string stringified_commit = this->serialize_deterministically_json_ad(store);

            // Create the public key object
            auto peer_public_key = ring::signature::UnparsedPublicKey(
                &ring::signature::ED25519,
                agent_pubkey
            );

            // Decode the signature
            std::vector<uint8_t> signature_bytes = base64::decode<std::vector<uint8_t>>(*signature);

            try {
                // Verify the signature
                peer_public_key.verify(
                    reinterpret_cast<const uint8_t*>(stringified_commit.c_str()),
                    stringified_commit.length(),
                    signature_bytes.data(),
                    signature_bytes.size()
                );
            } catch (const std::exception& e) {
                throw std::runtime_error(
                    "Incorrect signature for Commit. This could be due to an error during signing "
                    "or serialization of the commit. Stringified commit: " + stringified_commit
                );
            }
        }


        if (validate_timestamp) {
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            int64_t acceptable_ms_difference = 10000;
            if (created_at > now + acceptable_ms_difference) {
                throw std::runtime_error(
                    "Commit CreatedAt timestamp must lie in the past. Check your clock. "
                    "Timestamp now: " + std::to_string(now) + 
                    " CreatedAt is: " + std::to_string(created_at)
                );
            }
        }

        auto commit_resource = this->into_resource(store);
        
        std::shared_ptr<Resource> resource;
        try {
            *resource = *store->get_resource(subject);
        } catch (...) {
            *resource = subject;
        }

        if (set) {
            for (const auto& [prop, val] : *set) {
                resource->set_propval(prop, val, store);
            }
        }

        if (remove) {
            for (const auto& prop : *remove) {
                resource->remove_propval(prop);
            }
        }

        if (validate_schema) {
            resource->check_required_props(store);
        }

        if (validate_rights) {
            this->modify_parent(resource, store);

            if (!check_write(store, resource, signer)) {
                throw std::runtime_error(
                    "Agent " + signer + " is not permitted to edit " + subject + 
                    ". There should be a write right referring to this Agent in this Resource or its parent."
                );
            }
            std::cout << "This should not happen!" << std::endl;
        }

        if (destroy && *destroy) {
            store->remove_resource(subject);
            return commit_resource;
        }

        store->add_resource(commit_resource);
        store->add_resource(resource);

        return commit_resource;
    }

private:
    void modify_parent(std::shared_ptr<Resource>& resource, std::shared_ptr<Storelike>& store) {
        if (!resource->get(urls::PARENT)) {
            auto default_parent = store->get_self_url();
            if (!default_parent) {
                throw std::runtime_error("There is no self_url set, and no parent in the Commit. The commit can not be applied.");
            }
            resource->set_propval(urls::PARENT, Value(default_parent.value()), store);
        }
    }

    // Other methods like into_resource, check_write would need to be implemented
    std::shared_ptr<Resource> into_resource(std::shared_ptr<Storelike>& store) {
        std::string commit_subject;
        if (signature) {
            commit_subject = store->get_base_url() + "/commits/" + *signature;
        } else {
            int64_t now = datetime_helpers::now();
            commit_subject = store->get_base_url() + "/commitsUnsigned/" + std::to_string(now);
        }

        auto resource = std::make_shared<Resource>(commit_subject);

        resource->set_propval(urls::SUBJECT, Value(subject), store);
        resource->set_propval(urls::CREATED_AT, Value(created_at), store);
        resource->set_propval(urls::SIGNER, Value(signer), store);

        if (set) {
            resource->set_propval(urls::SET, Value(*set), store);
        }

        if (remove) {
            resource->set_propval(urls::REMOVE, Value(*remove), store);
        }

        if (destroy) {
            resource->set_propval(urls::DESTROY, Value(*destroy), store);
        }

        if (signature) {
            resource->set_propval(urls::SIGNATURE, Value(*signature), store);
        }

        return resource;
    }

    std::string serialize_deterministically_json_ad(const std::shared_ptr<Storelike>& store) {
        std::string s(sizeof(this), ' ');
        memcpy(s.data(), this, sizeof(*this));
        return s;
    }
};


bool check_write(
    Storelike& store,
    Resource& resource,
    const std::string& agent
) {
    // Check if the resource's write rights explicitly refers to the agent
    try {
        auto arr_val = resource.get(urls::WRITE);
        auto vec = arr_val->to_vec();
        if (std::find(vec.begin(), vec.end(), agent) != vec.end()) {
            return true;
        }
    } catch (const std::exception& e) {
        // If there's an error (e.g., property not found), we continue to check the parent
    }

    // Try the parents recursively
    try {
        auto val = resource.get(urls::PARENT);
        auto parent = store.get_resource(val->to_string());
        return check_write(store, *parent, agent);
    } catch (const std::exception& e) {
        // If there's an error (e.g., no parent), we return false
        return false;
    }
}
