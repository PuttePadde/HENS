from flask import Flask, jsonify, request, abort
from datetime import datetime, timedelta
import requests
import json
import sqlite3
from datetime import datetime

app = Flask(__name__)


# Global state variables
STATUS = "success"
HOURS_ARTIFICIAL = 0
SUNSET_HOUR = 0
EGG_JUST_NOW = False
FOOD_QUANTITY = 3.5
LIGHT_STATUS = False

PASS_UUID = "4569b4bb-eaf4-451d-b054-37f359a6d6b"

@app.route("/")
def gtfo():
    abort(403, description="Well, nothing to see here... GTFO.")

# API endpoint if an egg is laid.
@app.route("/egg_layed", methods=["POST"])
def egg_layed():
    # Get UUID and check if it's correct.
    UUID = request.args.get("UUID", type=str)
    if UUID != PASS_UUID:
        gtfo()	

    global EGG_JUST_NOW
    
    conn = sqlite3.connect("eggs.db")
    cursor = conn.cursor()

    data = json.loads(request.json["data"])

    egg_weight = data["egg_weight"]
    hen_weight = data["hen_weight"]
    lay_time   = data["lay_time"]

    # Add 1 hour cz. UTC timezone offset.
    current_time = datetime.now() + timedelta(hours=1)
    current_time = current_time.strftime('%Y-%m-%d %H:%M:%S')

    insert_query = """
    INSERT INTO eggs (date_time, hen_weight, egg_weight, food_quantity, light_on, light_hours, lay_time)
    VALUES (?, ?, ?, ?, ?, ?, ?);
    """

    # Use parameterized request to #€BD( SQL-injection.
    values = (current_time, hen_weight, egg_weight, FOOD_QUANTITY, SUNSET_HOUR, HOURS_ARTIFICIAL, lay_time)

    try:
        cursor.execute(insert_query, values)
        conn.commit()
        print("Record inserted successfully.")
    except sqlite3.Error as e:
        print(f"An error occurred: {e}")
    finally:
        conn.close()

    # New egg laid.
    EGG_JUST_NOW = True
    return "OK", 200
    

# Get sun interval and calculate artificial light.
@app.route("/get_interval", methods=["GET"])
def get_interval():
    # Get UUID and check if it's correct.
    UUID = request.args.get("UUID", type=str)
    if UUID != PASS_UUID:
        gtfo()	

    global HOURS_ARTIFICIAL, SUNSET_HOUR

    # API URL
    api_url = "https://api.sunrise-sunset.org/json?lat=55.8457057&lng=11.5298741&date=today&tzid=Europe/Copenhagen&formatted=0"

    try:
        # Fetch sun up/down data.
        response = requests.get(api_url)
        response.raise_for_status()
        data = response.json()

        # Parse JSON results.
        results = data.get("results", {})
        day_length_sec = results.get("day_length", 0)
        sunset_time = results.get("sunset", "")

        # Convert day length to whole hours.
        hours = day_length_sec // 3600

        # Calculate artificial light for 16 hours light in total.
        HOURS_ARTIFICIAL = 16 - int(hours)

        # Extract the sunset hour in 24-hour format.
        sunset_datetime = datetime.fromisoformat(sunset_time[:-6])  # Remove timezone offset
        SUNSET_HOUR = int(sunset_datetime.strftime("%H"))  # Get only the hour as integer

        # Return sunset hour and artificial light hours.
        return f"{SUNSET_HOUR};{SUNSET_HOUR + HOURS_ARTIFICIAL}"

    except requests.exceptions.RequestException as e:
        return jsonify({"status": "error", "message": f"API request failed: {e}"}), 500

# Set today's food supply.
@app.route("/set_food", methods=["GET"])
def set_food():
    # Get UUID and check if it's correct.
    UUID = request.args.get("UUID", type=str)
    if UUID != PASS_UUID:
        gtfo()	

    global FOOD_QUANTITY      
    # Get quantity parameter.
    quantity = request.args.get("quantity", type=float)

    if quantity is not None:
        FOOD_QUANTITY = quantity
        return jsonify({
            "status": "success",
            "message": "Food quantity updated",
            "new_food_quantity": FOOD_QUANTITY
        })
    else:
        return jsonify({
            "status": "error",
            "message": "Quantity parameter is required"
        }), 400

# Status API endpoint.
@app.route("/nest_status", methods=['GET'])
def nest_status():

    # Get UUID and check if it's correct.
    UUID = request.args.get("UUID", type=str)
    if UUID != PASS_UUID:
        gtfo()	

    global EGG_JUST_NOW
    conn = sqlite3.connect("eggs.db")
    cursor = conn.cursor()

    # Get hour as a string.
    hour = int(datetime.now().strftime('%H'))
    # Set light status.
    if hour >= SUNSET_HOUR and hour < SUNSET_HOUR + HOURS_ARTIFICIAL:
        LIGHT_STATUS = True
    else:
        LIGHT_STATUS = False

    # Retrieve the count of rows where the datetime matches today's date.
    cursor.execute("""
        SELECT COUNT(*) 
        FROM eggs
        WHERE DATE(date_time) = DATE('now')
    """)
    egg_count = cursor.fetchone()[0]

    # Retrieve the latest row.
    cursor.execute("""
        SELECT date_time, lay_time
        FROM eggs
        WHERE DATE(date_time) = DATE('now')
        ORDER BY date_time DESC
        LIMIT 1;
    """)

    result = cursor.fetchone()
    if result:
        latest_datetime = result[0] 
        lay_time = result[1]
    else:
        latest_datetime = 0
        lay_time = 0   


    # Close the database connection.
    conn.close()    
    data = {
        "status": STATUS,
        "data": {
            "egg_count": egg_count,
            "egg_just_now": EGG_JUST_NOW,
            "last_egg_datetime": latest_datetime,
            "light_status": LIGHT_STATUS,
            "lay_time": lay_time,
            "food_quantity": FOOD_QUANTITY,
        }
    }
    EGG_JUST_NOW = False
    # Return JSON response
    return jsonify(data)

if __name__ == "__main__":
    app.run(debug=True)
