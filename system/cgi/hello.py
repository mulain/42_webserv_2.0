#!/usr/bin/python3

import os, sys, cgi

request_method = os.environ.get("REQUEST_METHOD", "N/A")

if request_method == "POST":
	sys.stderr.write("Python: processing POST request.\n")
	form = cgi.FieldStorage() # python will automatically read from stdin if no param is given
	first_name = form.getvalue('first_name', 'none')
	last_name  = form.getvalue('last_name', 'none')
elif request_method == "GET":
	sys.stderr.write("Python: processing GET request.\n")
	query_string = os.environ.get("QUERY_STRING", "")
	query_pairs = query_string.split("&")

	query_parameters = {}
	for pair in query_pairs:
		key, value = pair.split("=")
		query_parameters[key] = value
	first_name = query_parameters.get('first_name', 'none')
	last_name = query_parameters.get('last_name', 'none')
else:
	sys.stderr.write("Python: Invalid request method.\n")
	exit(1)


html_content = """
<!DOCTYPE html>
<html>

<head>
	<title>Python CGI Example</title>
	<link rel="stylesheet" type="text/css" href="/styles.css"/>
</head>

<body>
	<h1>Python CGI:<br>{}</h1>
	<p>Hello {} {}!</p>
</body>

</html>
"""

print(html_content.format(request_method, first_name, last_name))
sys.stderr.write("Python: request processed.\n")