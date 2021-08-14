import java.io.File;
import java.io.File.*;
import java.io.FileNotFoundException;
import java.io.FileNotFoundException;
import java.lang.reflect.Array;
import java.util.*;
import java.util.List;

public class PoisonedReverse {
    public static void main(String[] args) {

        ArrayList<Router> allRouter = new ArrayList<>();
        ArrayList<Update> updateInformation = new ArrayList<>();
        PoisonedReverse pr = new PoisonedReverse();
        PoisonedReverse.Change change = pr.new Change(true);
        PoisonedReverse.Change hasThirdPart = pr.new Change(false);

        PoisonedReverse.Process process = pr.new Process();

        String path = "src/input";
//        process.readFiles(path, hasThirdPart);
        process.readFiles(hasThirdPart);

        allRouter = process.passRouter();
        allRouter = process.sortRouter(allRouter);
        process.initialRouter(allRouter);
        process.completeDistanceTB(allRouter);
        process.updateRoutingTable(allRouter);
        process.printInitialStatus(allRouter, 0);

        int times = 1;
//        while ( change.isIfChange() ) {
        while ( true ) {
            change.setIfChange(false);
            updateInformation = process.passMessage(allRouter);
            allRouter = process.updateDistanceTable(updateInformation, allRouter, change);
            allRouter = process.updateRoutingTable(allRouter);

            if( change.isIfChange() ) {
                process.printInitialStatus(allRouter, times);
                times ++;
                updateInformation.clear();
            }
            else
            {
                process.printRouter(allRouter);
                break;
            }
        }

        int part = 2;
//        if (hasThirdPart.isIfChange()) {
        while (!process.thirdSection.isEmpty()) {
//            System.out.println("3 part part");
            process.updateSecondInfo(allRouter, part);
            process.deleteInfo(part);
            part++;
            process.printInitialStatus(allRouter, times);
            times ++;
            while(true) {
                change.setIfChange(false);
                updateInformation = process.passMessage(allRouter);
                allRouter = process.updateDistanceTable(updateInformation, allRouter, change);
                allRouter = process.updateRoutingTable(allRouter);
                if( change.isIfChange() ) {
                    process.printInitialStatus(allRouter, times);
                    times ++;
                    updateInformation.clear();
                }
                else
                {
                    process.printRouter(allRouter);
                    break;
                }
            }
        }
//        System.out.println("3 Part " + process.updateStatus.isEmpty());
        System.exit(1);
    }

    private class Change {
        private boolean ifChange;

        public boolean isIfChange() {
            return ifChange;
        }

        public void setIfChange(boolean ifChange) {
            this.ifChange = ifChange;
        }

        public Change(boolean ifChange) {
            this.ifChange = ifChange;
        }

        public Change() {

        }
    }

    private class Router implements Comparable<Router>{
        private String name;
        private ArrayList<Router> neighbour;
        private ArrayList<Router> destination;
        private ArrayList<Road> routerRoad;
        private ArrayList<Road> distanceTable;
        private ArrayList<Road> routingTable;

        public void addNeighbour (Router router) {
            this.neighbour.add(router);
        }

        public void addDestination (Router router) {
            this.destination.add(router);
        }

        public void addRoad(Road road) {
            this.routerRoad.add(road);
        }

        public void addDistanceTable(Road road) {
            this.distanceTable.add(road);
        }

        public void addRoutingTable(Road road) { this.routingTable.add(road); }


        public int compareTo(Router compares) {
            int compareInt = this.name.compareTo(compares.getName());
            if (compareInt < 0) return -1;
            if (compareInt > 0) return 1;
            return 0;
        }

        public Router() {

        }

        public Router(String name) {
            this.name = name;
            neighbour = new ArrayList<>();
            destination = new ArrayList<>();
            distanceTable = new ArrayList<>();
            routingTable = new ArrayList<>();
            routerRoad = new ArrayList<>();
        }

        public void PrintRouterTable(){

        }


        public String getName() {
            return name;
        }

        public ArrayList<Router> getNeighbour() {
            return neighbour;
        }

        public ArrayList<Router> getDestination() {
            return destination;
        }

        public ArrayList<Road> getDistanceTable() {
            return distanceTable;
        }

        public ArrayList<Road> getRoutingTable() {
            return routingTable;
        }

        public void setName(String name) {
            this.name = name;
        }

        public void setNeighbour(ArrayList<Router> neighbour) {
            this.neighbour = neighbour;
        }

        public void setDestination(ArrayList<Router> destination) {
            this.destination = destination;
        }

        public void setDistanceTable(ArrayList<Road> distanceTable) {
            this.distanceTable = distanceTable;
        }

        public void setRoutingTable(ArrayList<Road> routingTable) {
            this.routingTable = routingTable;
        }

        public ArrayList<Road> getRouterRoad() {
            return routerRoad;
        }

        public void setRouterRoad(ArrayList<Road> routerRoad) {
            this.routerRoad = routerRoad;
        }

        @Override
        public String toString() {
            return "Router{" +
                    "name='" + name + '\'' +
                    ", \nneighbour=" + neighbour +
                    ", \ndestination=" + destination +
                    ", \nrouterRoad=" + routerRoad +
                    ", \ndistanceTable=" + distanceTable +
                    ", \nroutingTable=" + routingTable +
                    '}';
        }
    }

    private class Process {

        ArrayList<String> routerName = new ArrayList<>();
        ArrayList<String> startStatus = new ArrayList<>();
        ArrayList<String> updateStatus = new ArrayList<>();
        ArrayList<InputData> thirdSection = new ArrayList<>();

        public void readFiles(String path, Change hasThirdPart) {

//        TopologyUpdates topologyUpdates = new TopologyUpdates();

//      https://www.w3schools.com/java/java_files_read.asp
            try {
                File file = new File(path);
                Scanner scanner = new Scanner(file);
                int i = 0;

//          part: 0 routerName, 1 startStatus, 2 ++ updateStatus
                int part = 0;
//          when num_blank = 2, means that 2 empty/blank lines are received
                int num_blank = 0;
                InputData inputData;
                while (scanner.hasNextLine()) {
                    String tempData = scanner.nextLine();

                    if (!tempData.equals("")) {
                        num_blank = 0;
                        if (part == 0) {
//                        routerName.add(tempData);
                            if (!tempData.contains(" ")) {
                                routerName.add(tempData);
                            }
                            else {
                                System.out.println("Router name cannot contain spaces, program exit.");
                                System.exit(1);
                            }

                        }
                        else if (part == 1) {

                            if (checkName(tempData)) {
                                startStatus.add(tempData);
                            }
                            else {
                                System.out.println("Input details are incorrect, program exit.");
                                System.exit(1);
                            }
                        }
                        else if (part >= 2) {
                            if (checkName(tempData)) {
                                inputData = new InputData(String.valueOf(part));
                                inputData.setData(tempData);
                                thirdSection.add(inputData);
//                            thirdSection
                                updateStatus.add(tempData);
                                hasThirdPart.setIfChange(true);
//                            inputData.addData(tempData);
                            }
                        }
                    } else {
                        part++;
                        num_blank++;
                    }

                    System.out.println(tempData);
                    i++;
                    if (num_blank == 2){
                        break;
                    }
                }
//            System.out.println("i: " + i);
                scanner.close();
            } catch (FileNotFoundException e) {
                System.out.println("An error occurred.");
                e.printStackTrace();
            }



//        System.out.println();

//        return topologyUpdates;
        }

        private boolean checkName(String tempData) {
            boolean result = false;
            String[] input = tempData.split(" ");
            if (input.length != 3)
            {
                return result;
            }
            else {
                if ( exist(input[0]) && exist(input[1]) && ifDistance(input[2]) ) {
                    result = true;
                    return result;
                }
                else {
                    return result;
                }
            }
        }

        private boolean exist(String str) {
            boolean exist = false;
            for (String name : routerName) {
                if (str.equals(name)) {
                    exist = true;
                    break;
                }
            }
            return exist;
        }

        private boolean ifDistance(String str) {
            if(str.equals("-1") || str.matches("[0-9]+")) {
                return true;
            }
            else {
                return false;
            }
        }


        public void readFiles(Change hasThirdPart) {

            //      https://www.w3schools.com/java/java_files_read.asp
            try {
                Scanner scanner = new Scanner(System.in);
                int i = 0;

//          part: 0 routerName, 1 startStatus, 2 ++ updateStatus
                int part = 0;
//          when num_blank = 2, means that 2 empty/blank lines are received
                int num_blank = 0;
                InputData inputData;
                while (scanner.hasNextLine()) {
                    String tempData = scanner.nextLine();
                    if (!tempData.equals("")) {
                        num_blank = 0;
                        if (part == 0) {
//                        routerName.add(tempData);
                            if (!tempData.contains(" ")) {
                                routerName.add(tempData);
                            }
                            else {
                                System.out.println("Router name cannot contain spaces, program exit.");
                                System.exit(1);
                            }

                        }
                        else if (part == 1) {

                            if (checkName(tempData)) {
                                startStatus.add(tempData);
                            }
                            else {
                                System.out.println("Input details are incorrect, program exit.");
                                System.exit(1);
                            }
                        }
                        else if (part >= 2) {
                            if (checkName(tempData)) {
                                inputData = new InputData(String.valueOf(part));
                                inputData.setData(tempData);
                                thirdSection.add(inputData);
//                            thirdSection
                                updateStatus.add(tempData);
                                hasThirdPart.setIfChange(true);
//                            inputData.addData(tempData);
                            }
                        }
                    } else {
                        part++;
                        num_blank++;
                    }

//                System.out.println(tempData);

                    i++;
                    if (num_blank == 2){
//                    System.out.println("exit!");
                        break;
                    }
                }
//            System.out.println("i: " + i);
                scanner.close();
            } catch (Exception e) {
                System.out.println("An error occurred.");
                e.printStackTrace();
            }

        }

        public ArrayList<Router> passRouter() {
            ArrayList<Router> allRouter = new ArrayList<>();
            for (String rout : routerName){
                Router router = new Router(rout);
                allRouter.add(router);
//            allRouter.setName(rout);
            }
//        router.getName();

            return allRouter;
        }

        public void initialRouter(ArrayList<Router> allRouter) {
            if ( startStatus.isEmpty() ) {
                System.out.println("Input information is incomplete, program exit.");
                System.exit(1);
            }
            else {
                for (String status : startStatus) {
//          temp[0] = X, temp[1] = Y, temp[2] = distance
                    String[] temp = status.split(" ");
                    if (temp[2].equals("-1")) {
                        temp[2] = "-";
                    }
//            if (allRouter.contains())

                    for (Router all: allRouter) {
//
                        if (all.getName().equals(temp[0])) {
                            if (!checkRepeat(temp[0], temp[1], temp[2], allRouter)) {
                                Router rou = new Router(temp[1]);
                                all.addNeighbour(rou);
                                all.addDestination(rou);

                                Road distRoad = new Road(temp[1], temp[1], temp[2]);
                                Road routRoad = new Road(temp[0], temp[1], temp[2], temp[1]);
                                all.addRoad(distRoad);
                                all.addDistanceTable(distRoad);
                                all.addRoutingTable(routRoad);
                            }
                        }
                        else if (all.getName().equals(temp[1]))
                        {
                            if (!checkRepeat(temp[1], temp[0], temp[2], allRouter)) {
                                Router rou = new Router(temp[0]);
                                all.addNeighbour(rou);
                                all.addDestination(rou);
                                Road road = new Road(temp[0], temp[0], temp[2]);
                                all.addRoad(road);
                                all.addDistanceTable(road);
                                all.addRoutingTable(new Road(temp[1], temp[0], temp[2], temp[0]));
                            }
                        }
                    }
                }
                completeDestination(allRouter);
            }

        }

        private boolean checkRepeat(String s, String s1, String s2, ArrayList<Router> allRouter) {
            boolean result = false;
            check:
            for (Router router : allRouter) {
                if(s.equals(router.getName())) {
                    for (Router neigh : router.getNeighbour()) {
                        if (neigh.getName().equals(s1)){
                            result = true;
                            break check;
                        }
                    }
                }
            }
            return result;
        }

        private void completeDestination(ArrayList<Router> allRouter) {
            for (Router router : allRouter) {
                String router_Name = router.getName();
                for (String rouName : routerName) {
                    if (rouName.equals(router_Name)) continue;
                    else {
                        Boolean ifCreate = true;
                        for (Router dest : router.getDestination()) {
                            String destNa = dest.getName();
                            if (destNa.equals(rouName)) {
                                ifCreate = false;
                                break;
                            }
                        }
                        if (ifCreate) {
                            Router rout = new Router(rouName);
                            router.addDestination(rout);
                        }
                    }
                }
            }

        }

        public void printInitialStatus(ArrayList<Router> allRouter, int times) {

            for (Router router : allRouter) {
//          First line
                System.out.println("router " + router.getName() + " at t=" + times);
                ArrayList<Router> tempNeighbour = new ArrayList<>();
                ArrayList<Router> tempDestination = new ArrayList<>();

//          sort router's neighbour and destination according to alphabetical order
                Collections.sort(router.getNeighbour());
                Collections.sort(router.getDestination());
                tempNeighbour = router.getNeighbour();
                tempDestination = router.getDestination();

                int neighbourLength = tempNeighbour.size();

//          Second line
                System.out.print("     ");
                for (Router r : tempNeighbour) {
                    System.out.print(r.getName() + "    ");
                }
                System.out.println("");

//          Remained lines
                for (Router r : tempDestination) {
                    System.out.print(r.getName() + "   ");

                    for (int i = 0; i < neighbourLength; i++) {
                        String cost = "";
                        for (Road distance : router.getDistanceTable()) {
                            if( distance.getNeighbour().equals(tempNeighbour.get(i).getName()) && distance.getDestination().equals(r.getName()) ){
                                cost = distance.getDistance();
//                            System.out.print(cost + "   ");
                            }
                        }
                        if (!cost.equals("")) {
                            System.out.print(cost + "    ");
                        }
                        else{
                            System.out.print("INF" + "    ");
                        }
                    }
                    System.out.println("");
                }
                System.out.println("");
            }
        }

        public ArrayList<Update> passMessage(ArrayList<Router> allRouter) {
            ArrayList<Update> updates = new ArrayList<>();
            for (Router router : allRouter) {
                for (Router destination : router.getDestination()) {
                    for (Router neighbour : router.getNeighbour()) {

//                  cost from neighbour to destination
                        String costFromNtoD = "-1";
//                  find the cost in router's distance table, if not find, the cost is infinity
                        for (Road distanceTB : router.getDistanceTable()) {
//                      distance table's destination
                            String disTBDes = distanceTB.getDestination();
//                      distance table's neighbour
                            String disTBNei = distanceTB.getNeighbour();
//                      find the cost from original start router via neighbour to destination
                            if( (destination.getName().equals(disTBDes)) && (neighbour.getName().equals(disTBNei)) ) {
                                costFromNtoD = distanceTB.getDistance();
                                break;
                            }
                        }

//                  this is all of the c(x,v) + dv(y), use this to find the smallest
                        int newCost = 0;
//                  c(x,v)
                        String dist = "-1";
//                  dv(y)
                        String distY = "0";

//                  find the c(x,v) in the distance table
                        for (Road roadself : router.getDistanceTable()) {
                            String neigh = neighbour.getName();
                            String roNeigh = roadself.getNeighbour();
                            String roDest = roadself.getDestination();

                            if (neigh.equals(roNeigh) && neigh.equals(roDest)) {
                                dist = roadself.getDistance();
                                break;
                            }
                        }

                        String desti = "";
//                  find the dx(y) in the routing table
                        First:
                        for (Router rout : allRouter) {
                            String neigh = neighbour.getName();
                            String dest = destination.getName();
                            if (neigh.equals(rout.getName())) {
                                for (Road road : rout.getRoutingTable()) {
                                    if (dest.equals(road.getDestination())) {
                                        distY = road.getDistance();
                                        desti = road.getDestination();
                                        break First;
                                    }
                                }
                            }
                        }
                        distY = poison(desti, distY, allRouter);

//                    int costFrom = 0;
                        boolean deal = false;
                        boolean dealINF = false;

                        if (!dist.equals("-") && !distY.equals("-") ) {
                            if (!dist.equals("INF") && !distY.equals("INF") ) {
                                newCost = (Integer.parseInt(dist)) + (Integer.parseInt(distY));
                            }
                            else {
                                dealINF = true;
                            }

//                        costFrom = Integer. parseInt(costFromNtoD);
                        }
                        else {
                            deal = true;
                        }

                        String nei = neighbour.getName();
                        String des = destination.getName();
                        String dis = "";
                        if(deal) {
                            dis = "-";
                        }
                        else if (dealINF) {
                            dis = "INF";
                        }
                        else {
                            dis = String.valueOf(newCost);
                        }

                        String currentRouter = router.getName();
                        Update up = new Update(currentRouter, des, nei, dis);
                        updates.add(up);
                    }
                }
            }
            return updates;
        }

        private String poison(String desti, String distY, ArrayList<Router> router) {
            First:
            for (Router rout : router) {
                for ( Road road : rout.getDistanceTable()) {
                    String destination = road.getDestination();
                    String neighbour = road.getNeighbour();
                    String distance = road.getDistance();
                    if (distY.equals(distance) && desti.equals(destination) && !desti.equals(neighbour)) {
                        distY = "INF";
                        break First;
                    }
                }
            }

            return distY;
        }


        public void printRouter(ArrayList<Router> allRouter) {
            for ( Router router : allRouter) {
                for ( Router dest : router.getDestination()) {
                    String name = router.getName();
                    String destination = dest.getName();
                    String min = "";
                    for ( Road road : router.getRoutingTable()) {
                        if (destination.equals(road.getDestination())) {
                            min = road.getDistance();
                            break;
                        }
                    }
                    ArrayList<Integer> cost = new ArrayList<>();
                    String via = "" ;
                    for (Road road : router.getDistanceTable()) {
                        if (destination.equals(road.getDestination()) && min.equals(road.getDistance()) ) {
                            via = road.getNeighbour();
                            break;
                        }
                    }
                    if (min.equals("-")) {
                        min = "unreachable";
                        System.out.println("router " + name + ": " + destination + " is " + min);
                    }
                    else {
                        System.out.println("router " + name + ": " + destination + " is " + min + " routing through " + via);
                    }
                }
            }
            System.out.println();


        /*for (Router router: afterRouter) {
            System.out.print("router " + router.getName() + " t=1" + "\n");
            System.out.print("     ");
            for (Router neighbour : router.getNeighbour()) {
                System.out.print(neighbour.getName() + "    ");
            }
            for (Router destination : router.getDestination()) {
                System.out.print(destination.getName() + "    ");
            }
            for(Road road : router.getDistanceTable()) {
                System.out.print(road.getDistance() + " ");
            }
            System.out.println("");
        }*/
        }

        public ArrayList<Router> updateDistanceTable (ArrayList<Update> updateInformation, ArrayList<Router> allRouter, Change change) {
//      According to the update information, update the router's distance table and routing table
            for (Router router : allRouter) {
                for (Router destination : router.getDestination()) {
                    ArrayList<Integer> distanceList = new ArrayList<>();
//              update distance table
                    for (Router neighbour : router.getNeighbour()) {
                        String name = router.getName();
                        String dest = destination.getName();
                        String neigh = neighbour.getName();

//                  in the distance table, according the neighbour and destination,
//                  if the new cost not equal the old cost, replace old one.
                        Update:
                        for(Update update : updateInformation) {
                            if (name.equals(update.getName()) && dest.equals(update.getDestination()) && neigh.equals(update.getNeighbour())) {
                                String dis = update.getCost();

                                boolean ifCreate = true;
                                for (Road road : router.getDistanceTable()){
                                    String nei = road.getNeighbour();
                                    String des = road.getDestination();
                                    if (nei.equals(neigh) && dest.equals(des)) {
                                        String dist = road.getDistance();
                                        ifCreate = false;
                                        if( !dis.equals(dist) ) {
                                            road.setDistance(dis);
                                            change.setIfChange(true);
                                            break Update;
                                        }
                                    }
                                }
//                          if the distance table is incomplete, for example, in t = 0, it's incomplete and add something
                                if (ifCreate) {
                                    Road ro = new Road(neigh, dest, dis);
                                    router.getDistanceTable().add(ro);
                                }
                            }
                        }

//                  find the shortest cost
//                    String nam = router.getName();
//                    String des = destination.getName();
//
//                    for(Update update : updateInformation) {
//                        if (nam.equals(update.getName()) && des.equals(update.getDestination()) ) {
//                            if (!update.getCost().equals("-")) {
//                                distanceList.add(Integer.parseInt(update.getCost()));
//                            }
//                        }
//                    }
                    }

                }
            }

            return allRouter;
        }

        public ArrayList<Router> updateRoutingTable (ArrayList<Router> allRouter) {
            completeRoutingTable(allRouter);

            for (Router router : allRouter) {
                for (Router destination : router.getDestination()) {
                    ArrayList<Integer> distanceList = new ArrayList<>();
                    String destina = destination.getName();
//              INF = 0 : no INF , INF = 1 : INF
                    int INF = 0;
//              MINUS = 0 : no "-", MINUS = 1 : "-"
                    int MINUS = 0;
//              find the costs
                    for (Road disTB : router.getDistanceTable()) {
                        String dest = disTB.getDestination();
                        String cost = disTB.getDistance();
//                  need to add special cases
                        if (cost.equals("-")) {
                            MINUS = 1;
                        }
                        else if (cost.equals("INF")) {
                            INF = 1;
                        }
                        else if (dest.equals(destina )) {
                            distanceList.add(Integer.parseInt(cost));
                        }
                    }

//              update the routing table
                    for (Road ro : router.getRoutingTable()) {
                        String de = ro.getDestination();
                        if (!distanceList.isEmpty()) {
                            int min = Collections.min(distanceList);
                            if ( de.equals(destina) ) {
                                String roMin = ro.getDistance();

//                            if (min != Integer.parseInt((roMin))) {
                                ro.setDistance(String.valueOf(min));
                                break;
//                            }
                            }
                        }
                        else {
                            if ( de.equals(destina) ) {
                                if (INF == 1) {
                                    ro.setDistance("INF");
                                }
                                else if (MINUS == 1) {
                                    ro.setDistance("-");
                                }
                            }

                        }

                    }
                }
            }
            return allRouter;
        }

        private void completeRoutingTable(ArrayList<Router> allRouter) {
            for (Router router : allRouter) {
                String routName = router.getName();

                for (Router dest : router.getDestination()) {
                    String destination = dest.getName();
                    boolean ifCreate = true;
                    for(Road road : router.getRoutingTable()) {
                        String roadName = road.getDestination();
                        if(roadName.equals(destination)) {
                            ifCreate = false;
                            break;
                        }
                    }
                    if(ifCreate) {
                        Road road = new Road(routName, destination, "INF");
                        router.getRoutingTable().add(road);
                    }
                }
            }
        }

        public String getKey(HashMap<String, Integer> map, int value){
            String key="";
            for (HashMap.Entry<String, Integer> entry : map.entrySet()) {

                if(value == entry.getValue()) {
                    key=entry.getKey();
                }
            }
            return key;
        }

        //    updateStatus
        public void updateSecondInfo(ArrayList<Router> allRouter, int part) {
            for (InputData inputData : thirdSection) {
                if (part == Integer.parseInt(inputData.getName())) {
                    String update = inputData.getData();
                    String[] temp = update.split(" ");
                    for (Router router : allRouter) {
                        String updateName = temp[0];
                        String updateDest = temp[1];
                        String updateDis = temp[2];
                        String diff = "0";
                        if (updateName.equals(router.getName())) {
//                  first time, update the distance table's neighbour equals to destination.
//                  Example: X --> Z via Z
                            for (Road disTB : router.getDistanceTable()) {
                                String disNei = disTB.getNeighbour();
                                String disDest = disTB.getDestination();
                                if (updateDest.equals(disNei) && updateDest.equals(disDest)) {
                                    String distance = disTB.getDistance();
                                    if (updateDis.equals("-1") ) {
                                        diff = "-";
                                        updateDis = "-";
                                    }
                                    else {
                                        if (!distance.equals("-")) {
                                            diff = String.valueOf(Integer.parseInt(distance) - Integer.parseInt(updateDis));
                                        }
                                        else {
                                            diff = "+";
                                        }
                                    }
                                    disTB.setDistance(updateDis);
                                    break;
                                }
                            }

//                  Second time, update other distance
                            for (Road disTB : router.getDistanceTable()) {
                                String disNei = disTB.getNeighbour();
                                String disDest = disTB.getDestination();
                                if (updateDest.equals(disNei) && !updateDest.equals(disDest)) {
                                    if(diff.equals("-")) {
                                        disTB.setDistance("-");
                                    }
                                    else if (diff.equals("+")) {
//                                    updateDis
                                        disTB.setDistance("INF");
                                    }
                                    else {
                                        String distance = disTB.getDistance();
                                        if (!distance.equals("INF")) {
                                            int newCost = Integer.parseInt(distance) - Integer.parseInt(diff);
                                            disTB.setDistance(String.valueOf(newCost));
                                        }
                                        else {
                                            disTB.setDistance("INF");
                                        }
                                    }
                                }
                            }

//                      update the routing table
                            for (Road routTB : router.getRoutingTable()) {
                                String dest = routTB.getDestination();
                                ArrayList<Integer> cost = new ArrayList<>();
                                int MINUS = 0;
                                int INF = 0;
                                for (Road disTB : router.getDistanceTable()) {
                                    if (dest.equals(disTB.getDestination())) {
                                        String distance = disTB.getDistance();
                                        if (!distance.equals("-") && !distance.equals("INF")) {
                                            cost.add(Integer.parseInt(distance));
                                        }
                                        else if(distance.equals("-")) {
                                            MINUS = 1;
                                        }
                                        else if(distance.equals("INF")) {
                                            INF = 1;
                                        }
                                    }
                                }
                                if (!cost.isEmpty()) {
                                    int min = Collections.min(cost);
                                    routTB.setDistance(String.valueOf(min));
                                    cost.clear();

                                } else {
                                    if (INF == 1 ) {
                                        routTB.setDistance("INF");
                                    }
                                    else {
                                        routTB.setDistance("-");
                                    }
                                }
                            }
                        }
                    }

                    for (Router router : allRouter) {
                        String updateName = temp[1];
                        String updateDest = temp[0];
                        String updateDis = temp[2];
                        String diff = "0";
                        if (updateName.equals(router.getName())) {
//                  first time, update the distance table's neighbour equals to destination.
//                  Example: X --> Z via Z
                            for (Road disTB : router.getDistanceTable()) {
                                String disNei = disTB.getNeighbour();
                                String disDest = disTB.getDestination();
                                if (updateDest.equals(disNei) && updateDest.equals(disDest)) {
                                    String distance = disTB.getDistance();
                                    if (updateDis.equals("-1") ) {
                                        diff = "-";
                                        updateDis = "-";
                                    }
                                    else {
                                        if (!distance.equals("-")) {
                                            diff = String.valueOf(Integer.parseInt(distance) - Integer.parseInt(updateDis));
                                        }
                                        else {
                                            diff = "+";
                                        }
                                    }
                                    disTB.setDistance(updateDis);
                                    break;
                                }
                            }

//                  Second time, update other distance
                            for (Road disTB : router.getDistanceTable()) {
                                String disNei = disTB.getNeighbour();
                                String disDest = disTB.getDestination();
                                if (updateDest.equals(disNei) && !updateDest.equals(disDest)) {
                                    if(diff.equals("-")) {
                                        disTB.setDistance("-");
                                    }
                                    else if (diff.equals("+")) {
//                                    updateDis
                                        disTB.setDistance("INF");
                                    }
                                    else {
                                        String distance = disTB.getDistance();
                                        if(!distance.equals("INF")) {
                                            int newCost = Integer.parseInt(distance) - Integer.parseInt(diff);
                                            disTB.setDistance(String.valueOf(newCost));
                                        }
                                        else {
                                            disTB.setDistance("INF");
                                        }
                                    }
                                }
                            }

//                      update the routing table
                            for (Road routTB : router.getRoutingTable()) {
                                String dest = routTB.getDestination();
                                ArrayList<Integer> cost = new ArrayList<>();
                                int MINUS = 0;
                                int INF = 0;
                                for (Road disTB : router.getDistanceTable()) {
                                    if (dest.equals(disTB.getDestination())) {
                                        String distance = disTB.getDistance();
                                        if (!distance.equals("-") && !distance.equals("INF")) {
                                            cost.add(Integer.parseInt(distance));
                                        }
                                        else if(distance.equals("-")) {
                                            MINUS = 1;
                                        }
                                        else if(distance.equals("INF")) {
                                            INF = 1;
                                        }
                                    }
                                }
                                if (!cost.isEmpty()) {
                                    int min = Collections.min(cost);
                                    routTB.setDistance(String.valueOf(min));
                                    cost.clear();

                                } else {
                                    if (INF == 1 ) {
                                        routTB.setDistance("INF");
                                    }
                                    else {
                                        routTB.setDistance("-");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        public void deleteInfo(int part) {
            thirdSection.removeIf(t -> t.getName().equals(String.valueOf(part)));
        }


        public void completeDistanceTB(ArrayList<Router> allRouter) {
            for (Router router : allRouter) {
                for (Router destination : router.getDestination()) {
                    for (Router neighbour : router.getNeighbour()) {
                        String dest = destination.getName();
                        String neigh = neighbour.getName();
                        boolean notExist = true;
                        for (Road road : router.getDistanceTable()) {
                            String disNei = road.getNeighbour();
                            String disDes = road.getDestination();
                            if (disNei.equals(neigh) && disDes.equals(dest)) {
                                notExist = false;
                                break;
                            }
                        }
                        if (notExist) {
                            Road road = new Road(neigh, dest, "INF");
                            router.getDistanceTable().add(road);
                        }
                    }
                }
            }
            for (Router router : allRouter) {
                for (Router neighbour : router.getNeighbour()) {
                    String neigh = neighbour.getName();
                    boolean change = false;
                    for (Road disTB : router.getDistanceTable()) {
                        String nei = disTB.getNeighbour();
                        String cost = disTB.getDistance();
                        if (nei.equals(neigh) && cost.equals("-")) {
                            change = true;
                            break;
                        }
                    }

                    for (Road disTB : router.getDistanceTable()) {
                        String nei = disTB.getNeighbour();
                        String cost = disTB.getDistance();
                        if (change && nei.equals(neigh) && !cost.equals("-")) {
                            disTB.setDistance("-");
                        }
                    }
                }
            }

        }

        public ArrayList<Router> sortRouter(ArrayList<Router> allRouter) {
            Collections.sort(allRouter);
            return allRouter;
        }

    }

    private class InputData {
        private String name;
        private String data;

        public InputData(String name) {
            this.name = name;
        }

        public InputData () {

        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getData() {
            return data;
        }

        public void setData(String data) {
            this.data = data;
        }
    }

    private class Road {
        //    actually this is start router, because first time it has some error in naming,
//    and I used this attribute too many times, it's hard to change name.
        private String neighbour;
        private String destination;
        private String distance;
        //    this is the actually neighbour
        private String via;

        public boolean containsLocation(Collection<Road> c, String location) {
            for(Road o : c) {
                if(o != null && o.getNeighbour().equals(location)) {
                    return true;
                }
            }
            return false;
        }

        public Road() {

        }

        public Road(String neighbour, String destination, String distance) {
            this.neighbour = neighbour;
            this.destination = destination;
            this.distance = distance;

        }

        public Road(String neighbour, String destination, String distance, String via) {
            this.neighbour = neighbour;
            this.destination = destination;
            this.distance = distance;
            this.via = via;
        }

        public String getNeighbour() {
            return neighbour;
        }

        public void setNeighbour(String neighbour) {
            this.neighbour = neighbour;
        }

        public String getDestination() {
            return destination;
        }

        public void setDestination(String destination) {
            this.destination = destination;
        }

        public String getDistance() {
            return distance;
        }

        public void setDistance(String distance) {
            this.distance = distance;
        }

        public String getVia() {
            return via;
        }

        public void setVia(String via) {
            this.via = via;
        }

        @Override
        public String toString() {
            return "Road{" +
                    "neighbour='" + neighbour + '\'' +
                    ", destination='" + destination + '\'' +
                    ", distance='" + distance + '\'' +
                    ", via='" + via + '\'' +
                    '}';
        }
    }

    private class Update {
        private String name;
        private String destination;
        private String neighbour;
        private String cost;
//    private ArrayList<Road> newRoadInfo;

//    public void addInfo (Road road) {
//        newRoadInfo.add(road);
//    }


        public Update(String name, String destination, String neighbour, String cost) {
            this.name = name;
            this.destination = destination;
            this.neighbour = neighbour;
            this.cost = cost;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getDestination() {
            return destination;
        }

        public void setDestination(String destination) {
            this.destination = destination;
        }

        public String getNeighbour() {
            return neighbour;
        }

        public void setNeighbour(String neighbour) {
            this.neighbour = neighbour;
        }

        public String getCost() {
            return cost;
        }

        public void setCost(String cost) {
            this.cost = cost;
        }
    }

}


