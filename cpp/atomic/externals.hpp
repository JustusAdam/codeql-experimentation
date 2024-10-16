namespace base64 {
    template<typename T>
    T decode(const std::string& input) {
        T t;
        memcpy(&t, input.c_str(), sizeof(T));
        return t;
    }
}


namespace ring::signature {
    class Algorithm {};
    class UnparsedPublicKey {
        const Algorithm* algorithm;
        std::vector<uint8_t> key;
    public:
        UnparsedPublicKey(const ring::signature::Algorithm* algorithm, const std::vector<uint8_t>& key) : algorithm(algorithm), key(key) {}
        void verify(const uint8_t* message, size_t message_len, const uint8_t* signature, size_t signature_len) const {

        }
    };

    Algorithm ED25519;
}

// class Storelike {
//     std::string base_url;
//     std::optional<std::string> self_url;
//     std::unordered_map<std::string, std::shared_ptr<Resource>> resources;
// public: 
//     std::string& get_base_url() {
//         return base_url;
//     }

//     std::optional<std::string>& get_self_url() {
//         return self_url;
//     }

//     void add_resource(const std::shared_ptr<Resource>& resource) {
//         resources[resource->url] = resource;
//     }

//     void remove_resource(const std::string& url) {
//         resources.erase(url);
//     }

//     std::optional<Resource> get_resource(const std::string& url) {
//         auto it = resources.find(url);
//         if (it != resources.end()) {
//             return *it->second;
//         }
//         return std::nullopt;
//     }
// };