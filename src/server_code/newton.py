import requests

def request_handler(request):
    if request["method"] != "GET":
        return "Only accepts GET requests"
    try:
        operation = request["values"]["operation"]
    except:
        return "No operation was given"
    try:
        expression = request["values"]["expression"]
    except:
        return "No expression was given"
    try:
        value = request["values"]["value"]
    except:
        return "No value was given"

    expression = expression.replace("+", "%2B")
    data = requests.get(f"https://newton.now.sh/api/v2/{operation}/{expression}").json()
    if "result" not in data:
        return "Operation or expression entered incorrectly"
    result = data["result"].replace("x", value).replace("/", "(over)").replace("+", "%2B")
    data = requests.get(f"https://newton.now.sh/api/v2/simplify/{result}").json()
    if "result" not in data:
        return f"{result} could not be evaluated at {value}"
    split = data["result"].split("/")
    return split[0]