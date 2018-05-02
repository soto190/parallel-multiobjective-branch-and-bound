//
//  HandlerContainer3D.cpp
//  ParallelBaB
//
//  Created by Carlos Soto on 4/17/18.
//  Copyright © 2018 Carlos Soto. All rights reserved.
//

#include "HandlerContainer3D.hpp"

HandlerContainer3D::HandlerContainer3D():
rangeinx(nullptr),
rangeiny(nullptr),
rangeinz(nullptr),
maxinx(0),
maxiny(0),
maxinz(0),
numberOfElements(0),
grid(1, 1, 1) {

};

HandlerContainer3D::HandlerContainer3D(const HandlerContainer3D& toCopy):
maxinx(toCopy.getMaxRangeInX()),
maxiny(toCopy.getMaxRangeInY()),
maxinz(toCopy.getMaxRangeInZ()),
numberOfElements(toCopy.getNumberOfElements()),
activeBuckets(toCopy.getNumberOfActiveBuckets()),
unexploredBuckets(toCopy.getNumberOfUnexploredBuckets()),
disabledBuckets(toCopy.getNumberOfDisabledBuckets()),
grid(toCopy.getGrid()),
rangeinx(new double[toCopy.getCols()]),
rangeiny(new double[toCopy.getRows()]),
rangeinz(new double[toCopy.getDeep()]){

    for (int indexy = 0; indexy < toCopy.getRows(); ++indexy)
        rangeiny[indexy] = toCopy.rangeiny[indexy];

    for (int indexx = 0; indexx < toCopy.getCols(); ++indexx)
        rangeinx[indexx] = toCopy.rangeinx[indexx];

    for (int indexz = 0; indexz < toCopy.getDeep(); ++indexz)
        rangeinx[indexz] = toCopy.rangeinx[indexz];

    for (int obj = 0; obj < DIMENSIONS; ++obj)
        min_value_found_in_obj[obj] = toCopy.getBestValueFoundIn(obj);
}

HandlerContainer3D::HandlerContainer3D(unsigned int rows, unsigned int cols, unsigned int deep_n, double maxValX, double maxValY, double maxValZ):
grid(cols, rows, deep_n) {
    //    grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows) {
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    numberOfElements = 0;
    unexploredBuckets = rows * cols * deep_n;
    activeBuckets = 0;
    disabledBuckets = 0;

    rangeinx = new double[cols];
    rangeiny = new double[rows];
    rangeinz = new double[deep_n];

    maxinx = maxValX;
    maxiny = maxValY;
    maxinz = maxValZ;


    double rx = maxValX / cols;
    double ry = maxValY / rows;
    double rz = maxValZ / deep_n;

    rangeinx[0] = 0;
    rangeiny[0] = 0;
    rangeinz[0] = 0;

    for (int divs = 1; divs < cols; ++divs)
        rangeinx[divs] = rangeinx[divs - 1] + rx;

    for (int divs = 1; divs < rows; ++divs)
        rangeiny[divs] = rangeiny[divs - 1] + ry;

    for (int divs = 1; divs < deep_n; ++divs)
        rangeinz[divs] = rangeinz[divs - 1] + rz;

    for (int obj = 0; obj < DIMENSIONS; ++obj)
        min_value_found_in_obj[obj] = INFINITY;
}

HandlerContainer3D::~HandlerContainer3D() {
    delete[] rangeinx;
    delete[] rangeiny;
    delete[] rangeinz;

    pareto_front.clear();
    grid.clearAll();
}

HandlerContainer3D& HandlerContainer3D::operator()(unsigned int rows, unsigned int cols, unsigned int deep_n, double maxValX, double maxValY, double maxValZ, double minValX, double minValY, double minValZ) {
    /*
     if (maxValX < cols)
     cols = maxValX;
     if (maxValY < rows)
     rows = maxValY;
     */
    grid(cols, rows, deep_n);

    //grid(maxValX < cols?maxValX:cols, maxValY < rows?maxValY:rows);

    numberOfElements = 0;
    unexploredBuckets = rows * cols * deep_n;
    activeBuckets = 0;
    disabledBuckets = 0;

    if(rangeinx != nullptr)
        delete rangeinx;
    if(rangeiny != nullptr)
        delete rangeiny;
    if(rangeinz != nullptr)
        delete rangeinz;

    pareto_front.clear();

    rangeinx = new double[cols];
    rangeiny = new double[rows];
    rangeinz = new double[deep_n];

    maxinx = maxValX;
    maxiny = maxValY;
    maxinz = maxValZ;

    double rx = maxValX / cols;
    double ry = maxValY / rows;
    double rz = maxValZ / deep_n;

    rangeinx[0] = minValX;
    rangeiny[0] = minValY;
    rangeinz[0] = minValZ;

    for (int divs = 1; divs < cols; ++divs)
        rangeinx[divs] = rangeinx[divs - 1] + rx;

    for (int divs = 1; divs < rows; ++divs)
        rangeiny[divs] = rangeiny[divs - 1] + ry;

    for (int divs = 1; divs < deep_n; ++divs)
        rangeinz[divs] = rangeinz[divs - 1] + rz;

    for (int obj = 0; obj < DIMENSIONS; ++obj)
        min_value_found_in_obj[obj] = INFINITY;


    return *this;
}

void HandlerContainer3D::getCoordinateForSolution(const Solution &solution, int * coordinate) const {
    coordinate[0] = binarySearch(solution.getObjective(0), rangeinx, getCols());
    coordinate[1] = binarySearch(solution.getObjective(1), rangeiny, getRows());
    coordinate[2] = binarySearch(solution.getObjective(2), rangeiny, getDeep());
}

/**
 *
 * Sets a copy of the solution.
 * This could requiere a Mutex.
 * Returns 1 if the solution was added 0 in other case.
 *
 */
bool HandlerContainer3D::set(const Solution & solution, int x, int y, int z) {
    bool updated = false;
    FrontState state = grid.getStateOf(x, y, z);
    switch (state) {
        case FrontState::Unexplored:
            clearContainersDominatedBy(x, y, z);
            grid.addTo(solution, x, y, z);
            grid.setNonDominatedState(x, y, z);
            activeBuckets++;
            unexploredBuckets--;
            updated = true;
            break;

        case FrontState::NonDominated:
            updated = grid.addTo(solution, x, y, z);
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

void HandlerContainer3D::clearContainersDominatedBy(const int x, const int y, const int z) {
    for (int nRow = y + 1; nRow < grid.getNumberOfRows(); nRow++)
        for (int nCol = x + 1; nCol < grid.getNumberOfCols(); nCol++)
            for (int nDeep = z + 1; nDeep < grid.getNumberOfDeep(); nDeep++)

            if (grid.getStateOf(nCol, nRow, nDeep) == FrontState::dominated)
                nCol = grid.getNumberOfCols(); /** If the bucket in (nCol, nRow) is dominated then the exploration continues in next row. **/
            else
                clearContainer(nCol, nRow, nDeep);
}

void HandlerContainer3D::updateMinValueFound(const Solution &solution) {
    for (int objective = 0; objective < DIMENSIONS; ++objective)
        if (solution.getObjective(objective) < min_value_found_in_obj[objective])
            min_value_found_in_obj[objective] = solution.getObjective(objective);
}

double HandlerContainer3D::getBestValueFoundIn(int obj) const {
    return min_value_found_in_obj[obj];
}

/**
 * It uses a binary search tree to locate the bucket which will contain the new solution.
 **/
bool HandlerContainer3D::add(const Solution & solution) {

    /**TODO: This is a patch for the next issue. Sometimes the globalParetoBucket returns a solution with objetives {0, 0}. Consider to add a mutex when getting the vector<Solution> from the globalParetoBucker. **/
    for (unsigned int obj = 0; obj < solution.getNumberOfObjectives(); ++obj)
        if (solution.getObjective(obj) <= 1)
            return false;

    int coordinate[3];
    getCoordinateForSolution(solution, coordinate);
    return set(solution, coordinate[0], coordinate[1], coordinate[2]);
}

void HandlerContainer3D::clearContainer(int x, int y, int z) {
    if (grid.getSizeOf(x, y, z) > 0)
        activeBuckets--;

    disabledBuckets++;
    grid.clear(x, y, z);
}

/**
 * There are three states in gridState/BucketState:
 * 0: No explored.
 * 1: non-Dominated (Pareto front).
 * 2: Dominated.
 */
FrontState HandlerContainer3D::getStateOf(int x, int y, int z) const {
    return grid.getStateOf(x, y, z);
}

const ParetoFront& HandlerContainer3D::get(int x, int y, int z) const {
    return grid.getParetoFrontAt(x, y, z);
}

unsigned int HandlerContainer3D::getRows() const {
    return grid.getNumberOfRows();
}

unsigned int HandlerContainer3D::getCols() const {
    return grid.getNumberOfCols();
}

unsigned int HandlerContainer3D::getDeep() const {
    return grid.getNumberOfDeep();
}

unsigned int HandlerContainer3D::getMaxRangeInX() const {
    return maxinx;
}

unsigned int HandlerContainer3D::getMaxRangeInY() const {
    return maxiny;
}

unsigned int HandlerContainer3D::getMaxRangeInZ() const {
    return maxinz;
}

unsigned long HandlerContainer3D::getNumberOfElements() const {
    return grid.getNumberOfElements();
}

unsigned long HandlerContainer3D::getSizeOf(int x, int y, int z) const {
    return grid.getSizeOf(x, y, z);
}

unsigned long HandlerContainer3D::getNumberOfActiveBuckets() const {
    return activeBuckets;
}

unsigned long HandlerContainer3D::getNumberOfUnexploredBuckets() const {
    return unexploredBuckets;
}

unsigned long HandlerContainer3D::getNumberOfDisabledBuckets() const {
    return disabledBuckets;
}

void HandlerContainer3D::setNonDominatedState(int x, int y, int z) {
    grid.setNonDominatedState(x, y, z);
}

void HandlerContainer3D::setDominatedState(int x, int y, int z) {
    grid.setDominatedState(x, y, z);
}

void HandlerContainer3D::setUnexploredState(int x, int y, int z) {
    grid.setUnexploredState(x, y, z);
}

void HandlerContainer3D::print() const {
    grid.print();
}

void HandlerContainer3D::printGridSize() const {

    for (int nDeep = 0; nDeep < grid.getNumberOfDeep(); ++nDeep)
        for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
            printf("[%3d] ", nRow);
            for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol)
                if(grid.getStateOf(nCol, nRow, nDeep) == FrontState::Unexplored)
                    printf(" - ");
                else
                    printf("%3ld", grid.getSizeOf(nCol, nRow, nDeep));
            printf("\n");
        }
}

void HandlerContainer3D::printStates() const {
    FrontState state;
    for (int nDeep = 0; nDeep < grid.getNumberOfDeep(); ++nDeep)
        for (int nRow = grid.getNumberOfRows() - 1; nRow >= 0; --nRow) {
            printf("[%3d] ", nRow);
            for (int nCol = 0; nCol < grid.getNumberOfCols(); ++nCol) {
                state = grid.getStateOf(nCol, nRow, nDeep);
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
bool HandlerContainer3D::improvesTheGrid(const Solution &solution) {
    int bucketCoordinate[3];
    bool improves = false;
    getCoordinateForSolution(solution, bucketCoordinate);
    FrontState stateOfBucket = grid.getStateOf(bucketCoordinate[0], bucketCoordinate[1], bucketCoordinate[2]);
    switch (stateOfBucket) {
        case FrontState::dominated:
            improves = false;
            break;

        case FrontState::Unexplored:
            improves = true;
            break;

        case FrontState::NonDominated:
            improves = grid.produceImprovementInBucket(solution, bucketCoordinate[0], bucketCoordinate[1], bucketCoordinate[2]);
            break;
    }
    return improves;
}

bool HandlerContainer3D::improvesTheBucket(const Solution &solution, int x, int y, int z) {
    return grid.produceImprovementInBucket(solution, x, y, z);
}

const ParetoFront& HandlerContainer3D::generateParetoFront() {

    for (int bucket_z = 0; bucket_z < getDeep(); ++bucket_z)
        for (int bucket_y = 0; bucket_y < getRows(); ++bucket_y)
            for (int bucket_x = 0; bucket_x < getCols(); ++bucket_x) {
                FrontState state = grid.getStateOf(bucket_x, bucket_y, bucket_z);

                if (state == FrontState::NonDominated)
                    pareto_front += grid.getParetoFrontAt(bucket_x, bucket_y, bucket_z);

                else if (state == FrontState::dominated)
                    bucket_x = grid.getNumberOfCols();
            }

    return pareto_front;
}

const ParetoFront& HandlerContainer3D::getParetoFront() const {
    return pareto_front;
}

const GridContainer3D& HandlerContainer3D::getGrid() const {
    return grid;
}

double HandlerContainer3D::getMaxIn(int dimension) {
    if (dimension == 0)
        return maxinx;
    else if (dimension == 1)
        return maxiny;
    else if (dimension == 2)
        return maxinz;
    else
        return -1;
}

void HandlerContainer3D::clear() {
    grid.clearAll();
}
