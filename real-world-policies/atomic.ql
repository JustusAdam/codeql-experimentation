// Scope: 
// Everywhere

// Definitions:
// 1. "stored_resource" is each "resource" marked resource where:
//     A. There is a "commit" marked commit where:
//         a. "commit" goes to "resource"
//     and
//     B. There is a "store" marked sink where:
//         a. "resource" goes to "store"

// 2. "set_new_resource_properties" is each "set_permissions" marked new_resource where:
//     A. There is a "stored_resource" where:
//         a. For each "get_parent_permissions" marked parent_property:
//             i) "stored_resource" does not go to "get_parent_permissions"
//         and
//         b. "stored_resource" goes to "set_permissions"

// Policy:
// 1. For each "set_new_resource_properties":
//     A. For each "stored_resource":
//         a. If "stored_resource" goes to "set_new_resource_properties" then:
//             i) There is a "auth_check" marked check_rights where:
//                 A) "stored_resource" goes to "auth_check"
//                 and
//                 B) "auth_check" affects whether "set_new_resource_properties" happens

from DataFlow::Node stored_resource, DataFlow::Node resource, DataFlow::Node commit,
    DataFlow::Node set_new_resource_properties, DataFlow::Node set_permissions,
    

where
    stored_resource = resource and
    Flow1::flow(commit, resource) and

    (if SetPropFlow::flow(stored_resource, set_new_resource_properties) 
    then exists(DataFlow::Node auth_check | 
            AuthFlow::flow()
        )

    )

select set_new_resource_properties, stored_resource