import time

children_attr_name = 'Children'

def get_window_content(harness, sleep_before_dump_time = 0 ) :
    time.sleep(sleep_before_dump_time)
    body = {"ui": True, "getWindow": True}
    return harness.endpoint_request("developerMode", "get", body)['body']['dom']

def item_contains(body: dict, attr_name, attr_value ):
    if attr_name in body:
        return body[attr_name] == attr_value

#extends item_contains to check children
def item_contains_recursively(body :dict, attr_name, attr_value) :
    if len(body) == 1:
        return item_contains_recursively(next(iter(body.values())), attr_name, attr_value)

    if item_contains(body, attr_name, attr_value) :
        return True

    if children_attr_name in body:
        for child in body[children_attr_name] :
            value_found = item_contains_recursively(child, attr_name, attr_value)
            if value_found :
                return True
    return False

#extends item_contains_recursively to support list of attribute name-value pairs
def item_contains_multiple_recursively(body :dict, attr_name_value_pairs) :
    for name,value in attr_name_value_pairs:
        if not item_contains_recursively(body, name, value) :
            return False
    return True

#restricts item_contains_multiple_recursively to check only children
def item_has_child_that_contains_recursively(body :dict, attr_name_value_pairs) :
    if len(body) == 1:
        return item_has_child_that_contains_recursively(next(iter(body.values())), attr_name_value_pairs)

    if children_attr_name in body:
        for child in body[children_attr_name] :
            if item_contains_multiple_recursively(child, attr_name_value_pairs) :
                return True
    return False

#in children, finds child that contains given name-value attribute pairs and returns the child index
#usefull in ListView navigation
def get_child_number_that_contains_recursively(body :dict, attr_name_value_pairs) :
    if len(body) == 1:
        return get_child_number_that_contains_recursively(next(iter(body.values())), attr_name_value_pairs)

    if children_attr_name in body:
        child_index = 0
        for child in body[children_attr_name] :
            if item_contains_multiple_recursively(child, attr_name_value_pairs) :
                return child_index
            child_index += 1

    return -1

def find_parent(body: dict, child_name) -> dict :
    if len(body) == 1:
        return find_parent(next(iter(body.values())), child_name)

    if children_attr_name in body:
        for child in body[children_attr_name] :
            if next(iter(child)) == child_name :
                return body
            result = find_parent(next(iter(child.values())), child_name)
            if result :
                return result
    return {}

def find_child_that_contains(body: dict, attr_name, attr_value ) -> dict :
    if len(body) == 1:
        return find_child_that_contains(next(iter(body.values())), attr_name, attr_value)
    if children_attr_name in body:
        for child in body[children_attr_name] :
            if item_contains_recursively(child, attr_name, attr_value) :
                return child

    return {}

def find_item_depth_first(body: dict, attr_name ) -> dict:
    if attr_name in body:
        return body[attr_name]
    if children_attr_name in body:
        for child in body[children_attr_name] :
            result = find_item_depth_first(child, attr_name)
            if result:
                return result
    elif len(body) == 1:
        return find_item_depth_first(next(iter(body.values())), attr_name)
    return {}
