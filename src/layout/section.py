from http.client import NON_AUTHORITATIVE_INFORMATION
from operator import length_hint


class Section(object):

    def __init__(self, id, description, length, entrypoints, paths):
        self.id = id
        self.description = description
        self.length = length
        self.entrypoints = entrypoints
        self.paths = paths

    def calculate_remaining_coordinates(self):
        for entrypoint in self.entrypoints:
            if entrypoint.x_coord != None and entrypoint.y_coord != None:
                # Found the entrypoint with known coordinates
                known_entrypoint = entrypoint
                continue
            elif known_entrypoint != None:
                if "E" in known_entrypoint.id and "W" in entrypoint.id:
                    entrypoint.x_coord = known_entrypoint.x_coord - self.length
                elif "W" in known_entrypoint.id and "E" in entrypoint.id:
                    entrypoint.x_coord = known_entrypoint.x_coord + self.length
                elif ("W" in known_entrypoint.id and "W" in entrypoint.id) or ("E" in known_entrypoint.id and "E" in entrypoint.id):
                    entrypoint.x_coord = known_entrypoint.x_coord
                else:
                    raise Exception("Entrypoint doesn't contain W or E")



"""
    {
        "id": "1",
        "description" : "300mm suora osuus",
        "length" : 300,
        "entrypoints" : [
            {
                "id" : "1A",
                "connection" : "2A"
            },
            {
                "id" : "1B",
                "connection" : ""

            }
        ],
        "paths" : [
            ["1A", "1B"]
        ]
    },
    """