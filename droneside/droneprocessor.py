from dronecontrole import DronController
from argparse import ArgumentParser
import os


class Node:
    def __init__(self):
        self.ID = None
        self.X = None
        self.Y = None
        self.NeighLst = []
        self.CurrInd = None


class Drone:
    def __init__(self, id, nodeId):
        self.ID = id
        self.startNodeID = nodeId


class DronDataProcessor:
    def __init__(self, file_path):
        self.file_path = file_path
        self.droneType = None
        self.droneId = None
        self.dronesCount = None
        self.drones = []
        self.nodesCount = None
        self.nodes = []
        self.droneControl = DronController(20, 3)

    def readFileContent(self):
        try:
            with open(file_path, 'r') as file:
                self.droneType = int(file.readline().strip())
                self.droneId = int(file.readline().strip())
                dronesCount = int(file.readline().strip())
                for num in range(0, dronesCount):
                    aDroneID = int(file.readline().strip())
                    aStartNodeID = int(file.readline().strip())
                    self.drones.append(Drone(aDroneID, aStartNodeID))
                nodesCount = int(file.readline().strip())
                for num in range(0, nodesCount):
                    tmpNode = Node()
                    tmpNode.ID = int(file.readline().strip())
                    tmpNode.X = float(file.readline().strip())
                    tmpNode.Y = float(file.readline().strip())
                    neighCount = int(file.readline().strip())
                    for num1 in range(0, neighCount):
                        tmpNode.NeighLst.append(int(file.readline().strip()))
                    tmpNode.CurrInd = int(file.readline().strip())
                    self.nodes.append(tmpNode)
        except FileNotFoundError:
            print("File not found!")
        except IOError:
            print("Error reading the file!")

    def startProcessing(self, dronIp, isSimMode):
        vecDronesCurrPos = []
        vecDronesStartDir = []
        vecCompleted = []
        vecDroneLanded = []

        for drone in self.drones:
            vecDronesCurrPos.append(drone.startNodeID)
            vecDronesStartDir.append(self.nodes[drone.startNodeID].CurrInd)
            vecCompleted.append(False)

        def checkCompleted():
            res = True
            for droneRes in vecCompleted:
                res = res and droneRes
            return res

        def isDroneAlowedToFinish(currDroneNum):
            for i in range(len(self.drones)):
                if vecDronesCurrPos[currDroneNum] == self.drones[i].startNodeID and self.nodes[vecDronesCurrPos[currDroneNum]].CurrInd == vecDronesStartDir[i] and (vecCompleted[i] == False):
                    vecCompleted[i] = True
                    return True
            return False
        
        for droneNum in range(len(vecDronesCurrPos)):
            vecDroneLanded.append(False)

        self.droneControl.startup(dronIp, isSimMode)
        self.droneControl.goto(self.nodes[self.drones[self.droneId].startNodeID].X, self.nodes[self.drones[self.droneId].startNodeID].Y)
        print('Go to home')
        while not checkCompleted():
            for droneNum in range(len(vecDronesCurrPos)):
                if vecDroneLanded[droneNum]:
                    continue
                tmpInd = self.nodes[vecDronesCurrPos[droneNum]].CurrInd
                tmpInd += 1
                if tmpInd >= len(self.nodes[vecDronesCurrPos[droneNum]].NeighLst):
                    tmpInd = 0
                print(tmpInd)
                self.nodes[vecDronesCurrPos[droneNum]].CurrInd = tmpInd
                vecDronesCurrPos[droneNum] = self.nodes[vecDronesCurrPos[droneNum]].NeighLst[tmpInd]
                print('Node id is', vecDronesCurrPos[droneNum])
                if self.drones[droneNum].ID == self.droneId:
                    # Agit generate command for send drone to new pos
                    # to get coords see
                    print('Drone next coords is', self.nodes[vecDronesCurrPos[droneNum]].X, self.nodes[vecDronesCurrPos[droneNum]].Y)
                    self.droneControl.goto(self.nodes[vecDronesCurrPos[droneNum]].X, self.nodes[vecDronesCurrPos[droneNum]].Y)
                    if isDroneAlowedToFinish(droneNum):
                        vecDroneLanded[droneNum] = True
                        self.droneControl.missinon_end(mode='RTL')
                        exit(0)
                        # Agit generate command for landing
                else:
                    if isDroneAlowedToFinish(droneNum):
                        vecDroneLanded[droneNum] = True


if __name__ == '__main__':
    parser = ArgumentParser('Tool to travers the researched points.')
    parser.add_argument('-sim',
                        help='To run application on simulator.',
                        default=False,
                        action='store_true',
                        required=False
                        )
    parser.add_argument('-ip',
                        help='The drone IP or serial port path.',
                        default='/dev/serial0',
                        required=False,
                        type=str
                        )
    parser.add_argument('-data',
                        help='The drone data file path.',
                        required=True,
                        type=str
                        )
    args = parser.parse_args()

    file_path = os.path.abspath(args.data)  # Replace with the actual file path

    processor = DronDataProcessor(file_path)
    processor.readFileContent()

    print(processor.nodes)
    print('\n\n')
    print(processor.drones)

    processor.startProcessing(args.ip, args.sim)
