droneType = None
droneId = None
dronesCount = None
drones = []
nodesCount = None
nodes = []


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


def startProcessing():
    vecDronesCurrPos = []
    vecDronesStartDir = []
    vecCompleted = []

    for drone in drones:
        vecDronesCurrPos.append(drone.startNodeID)
        vecDronesStartDir.append(nodes[drone.startNodeID].CurrInd)
        vecCompleted.append(False)

    def checkCompleted():
        res = True
        for droneRes in vecCompleted:
            res = res and droneRes
        return res

    def isDroneAlowedToFinish(currDroneNum):
        for i in range(len(drones)):
            if vecDronesCurrPos[currDroneNum] == drones[i].startNodeID and nodes[vecDronesCurrPos[currDroneNum]].CurrInd == vecDronesStartDir[i] and (vecCompleted[i] == False):
                vecCompleted[i] = True
                return True
        return False

    print("strt node id: ", drones[0].startNodeID)
    print("start dir: ", vecDronesStartDir[0])
    print(nodes[drones[0].startNodeID].NeighLst)
    while not checkCompleted():
        for droneNum in range(len(vecDronesCurrPos)):
            tmpInd = nodes[vecDronesCurrPos[droneNum]].CurrInd
            tmpInd += 1
            if tmpInd >= len(nodes[vecDronesCurrPos[droneNum]].NeighLst):
                tmpInd = 0
            print(tmpInd)
            nodes[vecDronesCurrPos[droneNum]].CurrInd = tmpInd
            vecDronesCurrPos[droneNum] = nodes[vecDronesCurrPos[droneNum]].NeighLst[tmpInd]
            print(vecDronesCurrPos[droneNum])
            if drones[droneNum].ID == droneId:
                # Agit generate command for send drone to new pos
                print("drone go")
                #nodes[vecDronesCurrPos[droneNum]].X
                #nodes[vecDronesCurrPos[droneNum]].Y
                # to get coords see
                if isDroneAlowedToFinish(droneNum):
                    print("Allowed")
                    exit(0)
                    # Agit generate command for landing


if __name__ == '__main__':
    file_path = r"C:\Users\hayra\Desktop\IIAP WORK\new\build-DCIS-Desktop_Qt_6_5_1_MSVC2019_64bit-Debug\exploration_0\drone.data"  # Replace with the actual file path

    try:
        with open(file_path, 'r') as file:
            droneType = int(file.readline().strip())
            droneId = int(file.readline().strip())
            dronesCount = int(file.readline().strip())
            for num in range(0, dronesCount):
                aDroneID = int(file.readline().strip())
                aStartNodeID = int(file.readline().strip())
                drones.append(Drone(aDroneID, aStartNodeID))
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
                nodes.append(tmpNode)
    except FileNotFoundError:
        print("File not found!")
    except IOError:
        print("Error reading the file!")

    print(nodes)
    print('\n\n')
    print(drones)

    startProcessing()
