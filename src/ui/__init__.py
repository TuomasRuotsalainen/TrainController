import pygame, sys
from pygame.locals import *

class UI:
	def __init__(self):
		pygame.init()
		display = pygame.display.set_mode((400, 300))
		pygame.display.set_caption('Train controller')
		#pygame.screen.fill("white")

		white = [255, 255, 255]
		black = [0, 0, 0]

		display.fill(white)

		trackWidth = 5
		trackLength = 300
		yFromTop = 60
		xFromLeft = 0

		pygame.draw.rect(display, black, pygame.Rect(xFromLeft, yFromTop, trackLength, trackWidth))

		while True: # main game loop
			for event in pygame.event.get():
				if event.type == QUIT:
					pygame.quit()
					sys.exit()
			pygame.display.update()

	