class Entrypoint(object):

    def __init__(self, id, connection):
        self.id = id
        self.connection = connection
        self.x_coord = None
        self.y_coord = None

    def set_coordinates(self, x_coord, y_coord):
        self.x_coord = x_coord
        self.y_coord = y_coord