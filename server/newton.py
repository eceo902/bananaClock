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
	data = requests.get(f"https://newton.now.sh/api/v2/{operation}/{expression}").json()
	if "result" not in data:
		return "Operation or expression entered incorrectly"
	return data["result"]