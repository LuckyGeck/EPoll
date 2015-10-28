# EPoll
EPoll (**e**lectronic **poll**) - system for online voting.

# EPoll REST API

## List All Polls

* **REQUEST**
    * **URL:** `/polls`
    * **Method:** `GET`
* **RESPONSE**
    * **Content-Type:** `application/json`
    * **Code:** `200 HTTP_OK`
    * **Body:**
    ```
    {
        "polls": [
            {
                "created-at": "YYYY-MM-DD HH:MM:SS",
                "description": "Poll description",
                "id": "GUID"
            },
            ...
        ]
    }
    ```

## Get Poll Info

* **REQUEST**
    * **URL:** `/polls/(poll_id)`
    * **Method:** `GET`
* **RESPONSE**
    * **Content-Type:** `application/json`
    * **Code:**
        * Success - `200 HTTP_OK`
        * Fail - `404 HTTP_NOT_FOUND`
    * **Body:** (will be empty, if poll was not found)
    ```
    {
        "created-at": "YYYY-MM-DD HH:MM:SS",
        "description": "What is the answer to life the universe and everything?",
        "id": "GUID",
        "options": [
            {
                "text": "40",
                "votes": 2
            },
            {
                "text": "41",
                "votes": 2
            },
            {
                "text": "42",
                "votes": 100500
            },
            {
                "text": "43",
                "votes": 1
            },
            {
                "text": "44",
                "votes": 0
            }
        ]
    }
    ```

## Create Poll

* **REQUEST**
    * **URL:** `/polls`
    * **Method:** `POST`
    * **Body:**
    ```
    {
        "description": "To be or not to be?",
        "options": [
            "To be",
            "Not to be"
        ]
    }
    ```
* **RESPONSE**
    * **Code:**
        * Success - `201 HTTP_CREATED`
        * Fail - `400 HTTP_BAD_REQUEST`

## Vote for some option in specified poll

* **REQUEST**
    * **URL:** `/polls/(poll_id)/(option_idx)`
    * **Method:** `POST`
* **RESPONSE**
    * **Code:**
        * Success - `200 HTTP_OK`
        * Fail    - `404 HTTP_NOT_FOUND`