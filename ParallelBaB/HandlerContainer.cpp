//
//  HandlerContainer.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 2/28/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "HandlerContainer.hpp"

HandlerContainer::HandlerContainer():
rangeinx(nullptr),
rangeiny(nullptr),
maxinx(0),
maxiny(0),
//numberOfElements(0),
activeBuckets(0),
unexploredBuckets(0),
disabledBuckets(0),
grid(0,0) {

};

HandlerContainer::HandlerContainer(const HandlerContainer& toCopy):
maxinx(toCopy.getMaxValueInX()),
maxiny(toCopy.getMaxValueInY()),
grid(toCopy.getGrid()),
//numberOfElements(toCopy.getNumberOfElements()),
activeBuckets(toCopy.getNumberOfActiveBuckets()),
unexploredBuckets(toCopy.getNumberOfUnexploredBuckets()),
disabledBuckets(toCopy.getNumberOfDisabledBuckets()),
rangeinx(new double[toCopy.getCols()]),
rangeiny(new double[toCopy.getRows()]) {

    for (int indexy = 0; indexy < toCopy.getRows(); ++indexy)
        rangeiny[indexy] = toCopy.rangeiny[indexy];

    for (int indexx = 0; indexx < toCopy.getCols(); ++indexx)
        rangeinx[indexx] = toCopy.rangeinx[indexx];

    for (int obj = 0; obj < 2; ++obj)
        min_value_found_in_obj[obj] = toCopy.getBestValueFoundIn(obj);

    /*lb_x = minValX - ((maxinx - minValY) / (2 * toCopy.getCols()));
    lb_y = minValY - ((maxiny - minValY) / (2 * toCopy.getRows()));

    ub_x = maxinx + ((maxinx - minValY) / (2 * toCopy.getCols()));
    ub_y = maxValY + ((maxiny - minValY) / (2 * toCopy.getRows()));

    d_x = (lb_x - ub_x) / toCopy.getCols();
    d_y = (lb_y - ub_y) / toCopy.getRows();
*/
}

//HandlerContainer::HandlerContainer(unsigned int rows, unsigned int cols, double maxValX, double maxValY):
//unexploredBuckets(rows * cols),
//activeBuckets(0),
//disabledBuckets(0),
//maxinx(maxValX),
//maxiny(maxValY),
//grid(cols, rows),
//rangeinx(new double[cols]),
//rangeiny(new double[rows]) {
//    //    grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows) {
//    /*
//     if (maxValX < cols)
//     cols = maxValX;
//     if (maxValY < rows)
//     rows = maxValY;
//     */
//    //numberOfElements = 0;
//
//    lb_x = 0 - ((maxValX - 0) / (2 * cols));
//    lb_y = 0 - ((maxValY - 0) / (2 * rows));
//
//    ub_x = maxValX + ((maxValX - 0) / (2 * cols));
//    ub_y = maxValY + ((maxValY - 0) / (2 * rows));
//
//    double rx = maxValX / cols;
//    double ry = maxValY / rows;
//
//    rangeinx[0] = 0;
//    rangeiny[0] = 0;
//
//    for (int divs = 1; divs < cols; ++divs)
//        rangeinx[divs] = rangeinx[divs - 1] + rx;
//
//    for (int divs = 1; divs < rows; ++divs)
//        rangeiny[divs] = rangeiny[divs - 1] + ry;
//
//    for (int obj = 0; obj < 2; ++obj)
//        min_value_found_in_obj[obj] = INFINITY;
//}
//
HandlerContainer::~HandlerContainer() {
    delete[] rangeinx;
    delete[] rangeiny;
    pareto_front.clear();
    grid.clearAll();
}

HandlerContainer& HandlerContainer::operator()(unsigned int rows, unsigned int cols, double maxValX, double maxValY, int minValX, int minValY) {
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    grid(cols, rows);

    //grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows);

    //numberOfElements = 0;
    unexploredBuckets = rows * cols;
    activeBuckets = 0;
    disabledBuckets = 0;

    if(rangeinx != nullptr)
        delete [] rangeinx;
    if(rangeiny != nullptr)
        delete [] rangeiny;

    pareto_front.clear();

    maxinx = maxValX;
    maxiny = maxValY;
    mininx = minValX;
    mininy = minValY;

    lb_x = mininx - ((maxinx - mininx) / (2 * cols));
    ub_x = maxiny + ((maxinx - mininx) / (2 * cols));

    lb_y = mininy - ((maxiny - mininy) / (2 * rows));
    ub_y = maxiny + ((maxiny - mininy) / (2 * rows));

    d_x = (ub_x - lb_x)/ cols;
    d_y = (ub_y - lb_y)/ rows;

    /*193 143 307 297 11 15*/
    //std::cout << maxinx << ' ' << maxiny << ' ' << mininx << ' ' << mininy << ' ' << lb_x << ' ' << lb_y << ' ' << ub_x << ' ' << ub_y << ' '<< d_x << ' ' << d_y << ' ' << std::endl;
    rangeinx = new double[cols];
    rangeiny = new double[rows];

    double rx = maxValX / cols;
    double ry = maxValY / rows;

    rangeinx[0] = minValX;
    rangeiny[0] = minValY;

    for (int divs = 1; divs < cols; ++divs)
        rangeinx[divs] = rangeinx[divs - 1] + rx;

    for (int divs = 1; divs < rows; ++divs)
        rangeiny[divs] = rangeiny[divs - 1] + ry;

    for (int obj = 0; obj < 2; ++obj)
        min_value_found_in_obj[obj] = INFINITY;

    return *this;
}

void HandlerContainer::getCoordinateForSolution(const Solution &solution, int * coordinate) const {
    //int coordinate_new[2];
    /*193 143 307 297 11 15*/

   /* double value_x = (solution.getObjective(0) < mininx? mininx : solution.getObjective(0));
    value_x = (solution.getObjective(0) > maxinx? maxinx : solution.getObjective(0));

    double value_y = (solution.getObjective(1) < mininy? mininy : solution.getObjective(1));
    value_y = (solution.getObjective(1) > maxiny? maxiny : solution.getObjective(1));

    //cout << value_x << ' ' << value_y << ' ' << std::endl;
    coordinate_new[0] = floor((value_x - lb_x) / d_x);
    coordinate_new[1] = floor((value_y - lb_y) / d_y);
*/
    coordinate[0] = binarySearch(solution.getObjective(0), rangeinx, getCols());
    coordinate[1] = binarySearch(solution.getObjective(1), rangeiny, getRows());

    //cout << coordinate_new[0] << ' '<< coordinate_new[1] << ' '<<coordinate[0]<<' ' << coordinate[1] << std::endl;
}

/**
 *
 * Sets a copy of the solution.
 * This could requiere a Mutex.
 * Returns 1 if the solution was added 0 in other case.
 *
 */
bool HandlerContainer::set(const Solution & solution, int x, int y) {
    bool updated = false;
    FrontState state = grid.getStateOf(x, y);
    switch (state) {
        case FrontState::Unexplored:
            clearContainersDominatedBy(x, y);
            grid.addTo(solution, x, y);
            grid.setNonDominatedState(x, y);
            activeBuckets++;
            unexploredBuckets--;
            updated = true;
            break;

        case FrontState::NonDominated:
            updated = grid.addTo(solution, x, y);
            break;

        case FrontState::dominated:
            updated = false;
            /** If the bucket is dominated (State = 2) the element is not added. Then do nothing**/
            break;
    }

    if(updated)
        updateMinValueFound(solution);

    return updated;
}

void HandlerContainer::clearContainersDominatedBy(const int x, const int y) {
    for (int nRow = y + 1; nRow < grid.getNumberOfRows(); nRow++)
        for (int nCol = x + 1; nCol < grid.getNumberOfCols(); nCol++)
            if (grid.getStateOf(nCol, nRow) == FrontState::dominated)
                nCol = grid.getNumberOfCols(); /** If the bucket in (nCol, nRow) is dominated then the exploration continues in next row. **/
            else
                clearContainer(nCol, nRow);
}

void HandlerContainer::updateMinValueFound(const Solution &solution) {
    for (int objective = 0; objective < 2; ++objective)
        if (solution.getObjective(objective) < min_value_found_in_obj[objective])
            min_value_found_in_obj[objective] = solution.getObjective(objective);
}

double HandlerContainer::getBestValueFoundIn(int obj) const {
    return min_value_found_in_obj[obj];
}

/**
 * It uses a binary search tree to locate the bucket which will contain the new solution.
 **/
bool HandlerContainer::add(const Solution & solution) {

    /**TODO: This is a patch for the next issue. Sometimes the globalParetoBucket returns a solution with objetives {0, 0}. Consider to add a mutex when getting the vector<Solution> from the globalParetoBucker. **/
    for (unsigned int obj = 0; obj < solution.getNumberOfObjectives(); ++obj)
        if (solution.getObjective(obj) <= 1)
            return false;

    int coordinate[2];
    getCoordinateForSolution(solution, coordinate);
    return set(solution, coordinate[0], coordinate[1]);
}

void HandlerContainer::clearContainer(int x, int y) {
    if (grid.getSizeOf(x, y) > 0)
        activeBuckets--;

    disabledBuckets++;
    grid.clear(x, y);
}

/**
 * There are three states in gridState/BucketState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
FrontState HandlerContainer::getStateOf(int x, int y) const {
    return grid.getStateOf(x, y);
}

const ParetoFront& HandlerContainer::get(int x, int y) const {
    return grid.getParetoFrontAt(x, y);
}

unsigned int HandlerContainer::getRows() const {
    return grid.getNumberOfRows();
}

unsigned int HandlerContainer::getCols() const {
    return grid.getNumberOfCols();
}


double HandlerContainer::getMaxValueInX() const {
    return maxinx;
}

double HandlerContainer::getMaxValueInY() const {
    return maxiny;
}

unsigned long HandlerContainer::getSize() const {
    return grid.getSize();
}

unsigned long HandlerContainer::getSizeOf(int x, int y) const {
    return grid.getSizeOf(x, y);
}

unsigned long HandlerContainer::getNumberOfActiveBuckets() const {
    return activeBuckets;
}

unsigned long HandlerContainer::getNumberOfUnexploredBuckets() const {
    return unexploredBuckets;
}

unsigned long HandlerContainer::getNumberOfDisabledBuckets() const {
    return disabledBuckets;
}

unsigned long HandlerContainer::getNumberOfElements() const {
    return grid.getSize();
}

void HandlerContainer::setNonDominatedState(int x, int y) {
    grid.setNonDominatedState(x, y);
}

void HandlerContainer::setDominatedState(int x, int y) {
    grid.setDominatedState(x, y);
}

void HandlerContainer::setUnexploredState(int x, int y) {
    grid.setUnexploredState(x, y);
}

void HandlerContainer::print() const {
    grid.print();
}

void HandlerContainer::printGridSize() const {
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol)
            if(grid.getStateOf(nCol, nRow) == FrontState::Unexplored)
                printf(" - ");
            else
                printf("%3ld", grid.getSizeOf(nCol, nRow));
        printf("\n");
    }
}

void HandlerContainer::printStates() const {
    FrontState state;
    for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
        printf("[%3d] ", nRow);
        for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol) {
            state = grid.getStateOf(nCol, nRow);
            if(state == FrontState::Unexplored)
                printf(" - ");
            else
                printf("%3d", state);
        }
        printf("\n");
    }
}

/**
 * Stores a copy of the received solution.
 */
bool HandlerContainer::improvesTheGrid(const Solution &solution) {
    int bucketCoordinate[2];
    bool improves = false;
    getCoordinateForSolution(solution, bucketCoordinate);
    FrontState stateOfBucket = grid.getStateOf(bucketCoordinate[0], bucketCoordinate[1]);
    switch (stateOfBucket) {
        case FrontState::dominated:
            improves = false;
            break;

        case FrontState::Unexplored:
            improves = true;
            break;

        case FrontState::NonDominated:
            improves = grid.produceImprovementInBucket(solution, bucketCoordinate[0], bucketCoordinate[1]);
            break;
    }
    return improves;
}

bool HandlerContainer::improvesTheBucket(const Solution &solution, int x, int y) {
    return grid.produceImprovementInBucket(solution, x, y);
}

const ParetoFront& HandlerContainer::generateParetoFront() {

    for (int bucket_y = 0; bucket_y < getRows(); ++bucket_y)
        for (int bucket_x = 0; bucket_x < getCols(); ++bucket_x) {
            FrontState state = grid.getStateOf(bucket_x, bucket_y);

            if (state == FrontState::NonDominated)
                pareto_front += grid.getParetoFrontAt(bucket_x, bucket_y);

            else if (state == FrontState::dominated)
                bucket_x = grid.getNumberOfCols();
        }
    
    return pareto_front;
}

const ParetoFront& HandlerContainer::getParetoFront() const {
    return pareto_front;
}

const GridContainer& HandlerContainer::getGrid() const {
    return grid;
}

double HandlerContainer::getMaxIn(int dimension) {
    if (dimension == 0)
        return maxinx;
    else if (dimension == 1)
        return maxiny;
    else
        return -1;
}

void HandlerContainer::clear() {
    grid.clearAll();
}
