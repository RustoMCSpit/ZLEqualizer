// Copyright (C) 2023 - zsliu98
// This file is part of ZLEqualizer
//
// ZLEqualizer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// ZLEqualizer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEqualizer. If not, see <https://www.gnu.org/licenses/>.

#ifndef ZLEQUALIZER_SINGLE_FILTER_HPP
#define ZLEQUALIZER_SINGLE_FILTER_HPP

#include <juce_dsp/juce_dsp.h>
#include "coeff/design_filter.hpp"

namespace zlIIR {
    /** stl does not support constexpr log/pow, np.logspace(1, np.log10(22000), 800) */
    constexpr std::array<double, 800> frequencies = {
        1.00000000e+01, 1.00967885e+01, 1.01945137e+01, 1.02931849e+01,
        1.03928110e+01, 1.04934014e+01, 1.05949654e+01, 1.06975125e+01,
        1.08010521e+01, 1.09055938e+01, 1.10111473e+01, 1.11177225e+01,
        1.12253293e+01, 1.13339775e+01, 1.14436773e+01, 1.15544389e+01,
        1.16662726e+01, 1.17791886e+01, 1.18931976e+01, 1.20083100e+01,
        1.21245366e+01, 1.22418881e+01, 1.23603755e+01, 1.24800096e+01,
        1.26008017e+01, 1.27227630e+01, 1.28459046e+01, 1.29702382e+01,
        1.30957751e+01, 1.32225271e+01, 1.33505059e+01, 1.34797234e+01,
        1.36101916e+01, 1.37419225e+01, 1.38749285e+01, 1.40092218e+01,
        1.41448149e+01, 1.42817203e+01, 1.44199509e+01, 1.45595194e+01,
        1.47004387e+01, 1.48427220e+01, 1.49863825e+01, 1.51314333e+01,
        1.52778882e+01, 1.54257605e+01, 1.55750641e+01, 1.57258127e+01,
        1.58780204e+01, 1.60317013e+01, 1.61868697e+01, 1.63435399e+01,
        1.65017265e+01, 1.66614442e+01, 1.68227078e+01, 1.69855322e+01,
        1.71499325e+01, 1.73159241e+01, 1.74835222e+01, 1.76527426e+01,
        1.78236007e+01, 1.79961126e+01, 1.81702942e+01, 1.83461617e+01,
        1.85237314e+01, 1.87030198e+01, 1.88840434e+01, 1.90668192e+01,
        1.92513640e+01, 1.94376950e+01, 1.96258294e+01, 1.98157848e+01,
        2.00075787e+01, 2.02012290e+01, 2.03967536e+01, 2.05941706e+01,
        2.07934984e+01, 2.09947555e+01, 2.11979605e+01, 2.14031323e+01,
        2.16102899e+01, 2.18194526e+01, 2.20306397e+01, 2.22438709e+01,
        2.24591659e+01, 2.26765447e+01, 2.28960275e+01, 2.31176346e+01,
        2.33413867e+01, 2.35673044e+01, 2.37954087e+01, 2.40257208e+01,
        2.42582620e+01, 2.44930540e+01, 2.47301185e+01, 2.49694775e+01,
        2.52111533e+01, 2.54551681e+01, 2.57015448e+01, 2.59503061e+01,
        2.62014751e+01, 2.64550752e+01, 2.67111298e+01, 2.69696627e+01,
        2.72306979e+01, 2.74942596e+01, 2.77603723e+01, 2.80290607e+01,
        2.83003497e+01, 2.85742644e+01, 2.88508303e+01, 2.91300731e+01,
        2.94120186e+01, 2.96966930e+01, 2.99841227e+01, 3.02743344e+01,
        3.05673550e+01, 3.08632117e+01, 3.11619320e+01, 3.14635436e+01,
        3.17680744e+01, 3.20755527e+01, 3.23860070e+01, 3.26994662e+01,
        3.30159593e+01, 3.33355157e+01, 3.36581650e+01, 3.39839372e+01,
        3.43128625e+01, 3.46449714e+01, 3.49802948e+01, 3.53188637e+01,
        3.56607095e+01, 3.60058640e+01, 3.63543593e+01, 3.67062275e+01,
        3.70615014e+01, 3.74202140e+01, 3.77823985e+01, 3.81480885e+01,
        3.85173180e+01, 3.88901212e+01, 3.92665327e+01, 3.96465874e+01,
        4.00303206e+01, 4.04177680e+01, 4.08089653e+01, 4.12039490e+01,
        4.16027557e+01, 4.20054224e+01, 4.24119864e+01, 4.28224855e+01,
        4.32369577e+01, 4.36554416e+01, 4.40779759e+01, 4.45045998e+01,
        4.49353530e+01, 4.53702754e+01, 4.58094073e+01, 4.62527895e+01,
        4.67004631e+01, 4.71524697e+01, 4.76088512e+01, 4.80696500e+01,
        4.85349087e+01, 4.90046707e+01, 4.94789793e+01, 4.99578787e+01,
        5.04414134e+01, 5.09296281e+01, 5.14225681e+01, 5.19202792e+01,
        5.24228076e+01, 5.29301999e+01, 5.34425032e+01, 5.39597649e+01,
        5.44820332e+01, 5.50093564e+01, 5.55417835e+01, 5.60793639e+01,
        5.66221474e+01, 5.71701845e+01, 5.77235259e+01, 5.82822230e+01,
        5.88463277e+01, 5.94158922e+01, 5.99909695e+01, 6.05716129e+01,
        6.11578762e+01, 6.17498139e+01, 6.23474808e+01, 6.29509325e+01,
        6.35602249e+01, 6.41754145e+01, 6.47965585e+01, 6.54237144e+01,
        6.60569405e+01, 6.66962954e+01, 6.73418386e+01, 6.79936299e+01,
        6.86517298e+01, 6.93161993e+01, 6.99871002e+01, 7.06644945e+01,
        7.13484453e+01, 7.20390159e+01, 7.27362705e+01, 7.34402737e+01,
        7.41510908e+01, 7.48687878e+01, 7.55934313e+01, 7.63250884e+01,
        7.70638272e+01, 7.78097162e+01, 7.85628244e+01, 7.93232219e+01,
        8.00909792e+01, 8.08661674e+01, 8.16488586e+01, 8.24391254e+01,
        8.32370410e+01, 8.40426795e+01, 8.48561157e+01, 8.56774250e+01,
        8.65066836e+01, 8.73439685e+01, 8.81893573e+01, 8.90429285e+01,
        8.99047613e+01, 9.07749357e+01, 9.16535323e+01, 9.25406327e+01,
        9.34363193e+01, 9.43406751e+01, 9.52537839e+01, 9.61757307e+01,
        9.71066008e+01, 9.80464806e+01, 9.89954574e+01, 9.99536192e+01,
        1.00921055e+02, 1.01897854e+02, 1.02884108e+02, 1.03879907e+02,
        1.04885345e+02, 1.05900514e+02, 1.06925509e+02, 1.07960424e+02,
        1.09005357e+02, 1.10060403e+02, 1.11125661e+02, 1.12201229e+02,
        1.13287207e+02, 1.14383697e+02, 1.15490799e+02, 1.16608617e+02,
        1.17737253e+02, 1.18876814e+02, 1.20027405e+02, 1.21189131e+02,
        1.22362102e+02, 1.23546426e+02, 1.24742213e+02, 1.25949574e+02,
        1.27168620e+02, 1.28399466e+02, 1.29642225e+02, 1.30897012e+02,
        1.32163944e+02, 1.33443138e+02, 1.34734714e+02, 1.36038790e+02,
        1.37355489e+02, 1.38684932e+02, 1.40027242e+02, 1.41382544e+02,
        1.42750964e+02, 1.44132628e+02, 1.45527666e+02, 1.46936206e+02,
        1.48358379e+02, 1.49794317e+02, 1.51244153e+02, 1.52708022e+02,
        1.54186059e+02, 1.55678402e+02, 1.57185189e+02, 1.58706561e+02,
        1.60242657e+02, 1.61793621e+02, 1.63359597e+02, 1.64940729e+02,
        1.66537165e+02, 1.68149053e+02, 1.69776541e+02, 1.71419783e+02,
        1.73078928e+02, 1.74754133e+02, 1.76445551e+02, 1.78153340e+02,
        1.79877659e+02, 1.81618667e+02, 1.83376526e+02, 1.85151400e+02,
        1.86943451e+02, 1.88752848e+02, 1.90579758e+02, 1.92424350e+02,
        1.94286796e+02, 1.96167268e+02, 1.98065941e+02, 1.99982991e+02,
        2.01918595e+02, 2.03872934e+02, 2.05846189e+02, 2.07838543e+02,
        2.09850180e+02, 2.11881287e+02, 2.13932054e+02, 2.16002669e+02,
        2.18093326e+02, 2.20204218e+02, 2.22335540e+02, 2.24487492e+02,
        2.26660272e+02, 2.28854082e+02, 2.31069125e+02, 2.33305608e+02,
        2.35563737e+02, 2.37843722e+02, 2.40145775e+02, 2.42470109e+02,
        2.44816939e+02, 2.47186485e+02, 2.49578965e+02, 2.51994601e+02,
        2.54433618e+02, 2.56896242e+02, 2.59382701e+02, 2.61893227e+02,
        2.64428051e+02, 2.66987409e+02, 2.69571539e+02, 2.72180681e+02,
        2.74815076e+02, 2.77474969e+02, 2.80160606e+02, 2.82872238e+02,
        2.85610114e+02, 2.88374491e+02, 2.91165623e+02, 2.93983770e+02,
        2.96829194e+02, 2.99702158e+02, 3.02602929e+02, 3.05531776e+02,
        3.08488971e+02, 3.11474789e+02, 3.14489505e+02, 3.17533401e+02,
        3.20606758e+02, 3.23709861e+02, 3.26842999e+02, 3.30006462e+02,
        3.33200544e+02, 3.36425541e+02, 3.39681752e+02, 3.42969479e+02,
        3.46289028e+02, 3.49640706e+02, 3.53024825e+02, 3.56441698e+02,
        3.59891642e+02, 3.63374978e+02, 3.66892029e+02, 3.70443120e+02,
        3.74028582e+02, 3.77648747e+02, 3.81303951e+02, 3.84994534e+02,
        3.88720837e+02, 3.92483206e+02, 3.96281990e+02, 4.00117543e+02,
        4.03990219e+02, 4.07900378e+02, 4.11848383e+02, 4.15834600e+02,
        4.19859399e+02, 4.23923154e+02, 4.28026241e+02, 4.32169041e+02,
        4.36351939e+02, 4.40575322e+02, 4.44839583e+02, 4.49145116e+02,
        4.53492323e+02, 4.57881605e+02, 4.62313371e+02, 4.66788031e+02,
        4.71306001e+02, 4.75867699e+02, 4.80473549e+02, 4.85123979e+02,
        4.89819419e+02, 4.94560306e+02, 4.99347079e+02, 5.04180182e+02,
        5.09060065e+02, 5.13987179e+02, 5.18961982e+02, 5.23984935e+02,
        5.29056504e+02, 5.34177161e+02, 5.39347380e+02, 5.44567640e+02,
        5.49838426e+02, 5.55160228e+02, 5.60533538e+02, 5.65958856e+02,
        5.71436685e+02, 5.76967533e+02, 5.82551913e+02, 5.88190343e+02,
        5.93883347e+02, 5.99631452e+02, 6.05435193e+02, 6.11295107e+02,
        6.17211738e+02, 6.23185636e+02, 6.29217354e+02, 6.35307452e+02,
        6.41456495e+02, 6.47665053e+02, 6.53933704e+02, 6.60263027e+02,
        6.66653612e+02, 6.73106049e+02, 6.79620939e+02, 6.86198886e+02,
        6.92840499e+02, 6.99546396e+02, 7.06317198e+02, 7.13153533e+02,
        7.20056037e+02, 7.27025348e+02, 7.34062115e+02, 7.41166989e+02,
        7.48340630e+02, 7.55583704e+02, 7.62896883e+02, 7.70280844e+02,
        7.77736274e+02, 7.85263864e+02, 7.92864312e+02, 8.00538324e+02,
        8.08286611e+02, 8.16109893e+02, 8.24008895e+02, 8.31984350e+02,
        8.40036998e+02, 8.48167587e+02, 8.56376871e+02, 8.64665611e+02,
        8.73034576e+02, 8.81484544e+02, 8.90016297e+02, 8.98630628e+02,
        9.07328335e+02, 9.16110227e+02, 9.24977117e+02, 9.33929828e+02,
        9.42969191e+02, 9.52096045e+02, 9.61311236e+02, 9.70615619e+02,
        9.80010059e+02, 9.89495425e+02, 9.99072599e+02, 1.00874247e+03,
        1.01850593e+03, 1.02836389e+03, 1.03831727e+03, 1.04836698e+03,
        1.05851397e+03, 1.06875916e+03, 1.07910352e+03, 1.08954799e+03,
        1.10009356e+03, 1.11074120e+03, 1.12149189e+03, 1.13234664e+03,
        1.14330645e+03, 1.15437233e+03, 1.16554533e+03, 1.17682646e+03,
        1.18821678e+03, 1.19971735e+03, 1.21132923e+03, 1.22305350e+03,
        1.23489124e+03, 1.24684357e+03, 1.25891157e+03, 1.27109639e+03,
        1.28339913e+03, 1.29582095e+03, 1.30836301e+03, 1.32102645e+03,
        1.33381246e+03, 1.34672223e+03, 1.35975694e+03, 1.37291782e+03,
        1.38620608e+03, 1.39962296e+03, 1.41316969e+03, 1.42684755e+03,
        1.44065778e+03, 1.45460169e+03, 1.46868056e+03, 1.48289569e+03,
        1.49724841e+03, 1.51174004e+03, 1.52637194e+03, 1.54114546e+03,
        1.55606197e+03, 1.57112286e+03, 1.58632951e+03, 1.60168335e+03,
        1.61718580e+03, 1.63283829e+03, 1.64864228e+03, 1.66459924e+03,
        1.68071064e+03, 1.69697798e+03, 1.71340277e+03, 1.72998653e+03,
        1.74673080e+03, 1.76363714e+03, 1.78070711e+03, 1.79794230e+03,
        1.81534431e+03, 1.83291475e+03, 1.85065525e+03, 1.86856746e+03,
        1.88665303e+03, 1.90491366e+03, 1.92335102e+03, 1.94196684e+03,
        1.96076284e+03, 1.97974076e+03, 1.99890237e+03, 2.01824944e+03,
        2.03778376e+03, 2.05750716e+03, 2.07742145e+03, 2.09752850e+03,
        2.11783015e+03, 2.13832830e+03, 2.15902485e+03, 2.17992172e+03,
        2.20102085e+03, 2.22232419e+03, 2.24383373e+03, 2.26555145e+03,
        2.28747937e+03, 2.30961953e+03, 2.33197399e+03, 2.35454480e+03,
        2.37733408e+03, 2.40034393e+03, 2.42357649e+03, 2.44703391e+03,
        2.47071838e+03, 2.49463208e+03, 2.51877724e+03, 2.54315610e+03,
        2.56777092e+03, 2.59262398e+03, 2.61771758e+03, 2.64305407e+03,
        2.66863578e+03, 2.69446510e+03, 2.72054441e+03, 2.74687614e+03,
        2.77346273e+03, 2.80030665e+03, 2.82741039e+03, 2.85477646e+03,
        2.88240740e+03, 2.91030578e+03, 2.93847418e+03, 2.96691522e+03,
        2.99563154e+03, 3.02462580e+03, 3.05390068e+03, 3.08345892e+03,
        3.11330324e+03, 3.14343643e+03, 3.17386126e+03, 3.20458058e+03,
        3.23559722e+03, 3.26691407e+03, 3.29853403e+03, 3.33046003e+03,
        3.36269504e+03, 3.39524205e+03, 3.42810407e+03, 3.46128417e+03,
        3.49478540e+03, 3.52861089e+03, 3.56276378e+03, 3.59724722e+03,
        3.63206442e+03, 3.66721861e+03, 3.70271306e+03, 3.73855105e+03,
        3.77473591e+03, 3.81127100e+03, 3.84815970e+03, 3.88540545e+03,
        3.92301169e+03, 3.96098192e+03, 3.99931965e+03, 4.03802845e+03,
        4.07711191e+03, 4.11657364e+03, 4.15641733e+03, 4.19664665e+03,
        4.23726535e+03, 4.27827719e+03, 4.31968597e+03, 4.36149555e+03,
        4.40370980e+03, 4.44633262e+03, 4.48936799e+03, 4.53281990e+03,
        4.57669236e+03, 4.62098946e+03, 4.66571531e+03, 4.71087405e+03,
        4.75646988e+03, 4.80250702e+03, 4.84898974e+03, 4.89592237e+03,
        4.94330925e+03, 4.99115478e+03, 5.03946340e+03, 5.08823959e+03,
        5.13748788e+03, 5.18721283e+03, 5.23741907e+03, 5.28811124e+03,
        5.33929405e+03, 5.39097226e+03, 5.44315065e+03, 5.49583407e+03,
        5.54902740e+03, 5.60273558e+03, 5.65696360e+03, 5.71171648e+03,
        5.76699930e+03, 5.82281720e+03, 5.87917536e+03, 5.93607899e+03,
        5.99353338e+03, 6.05154387e+03, 6.11011583e+03, 6.16925471e+03,
        6.22896597e+03, 6.28925518e+03, 6.35012791e+03, 6.41158982e+03,
        6.47364661e+03, 6.53630404e+03, 6.59956792e+03, 6.66344413e+03,
        6.72793858e+03, 6.79305726e+03, 6.85880621e+03, 6.92519154e+03,
        6.99221941e+03, 7.05989602e+03, 7.12822767e+03, 7.19722069e+03,
        7.26688148e+03, 7.33721651e+03, 7.40823230e+03, 7.47993544e+03,
        7.55233258e+03, 7.62543045e+03, 7.69923582e+03, 7.77375554e+03,
        7.84899652e+03, 7.92496575e+03, 8.00167027e+03, 8.07911721e+03,
        8.15731374e+03, 8.23626713e+03, 8.31598469e+03, 8.39647383e+03,
        8.47774200e+03, 8.55979677e+03, 8.64264572e+03, 8.72629656e+03,
        8.81075704e+03, 8.89603500e+03, 8.98213836e+03, 9.06907509e+03,
        9.15685328e+03, 9.24548105e+03, 9.33496664e+03, 9.42531834e+03,
        9.51654455e+03, 9.60865372e+03, 9.70165440e+03, 9.79555522e+03,
        9.89036489e+03, 9.98609221e+03, 1.00827461e+04, 1.01803354e+04,
        1.02788693e+04, 1.03783569e+04, 1.04788074e+04, 1.05802302e+04,
        1.06826346e+04, 1.07860302e+04, 1.08904265e+04, 1.09958333e+04,
        1.11022603e+04, 1.12097173e+04, 1.13182145e+04, 1.14277617e+04,
        1.15383693e+04, 1.16500474e+04, 1.17628064e+04, 1.18766568e+04,
        1.19916091e+04, 1.21076740e+04, 1.22248624e+04, 1.23431849e+04,
        1.24626527e+04, 1.25832768e+04, 1.27050684e+04, 1.28280388e+04,
        1.29521994e+04, 1.30775618e+04, 1.32041375e+04, 1.33319383e+04,
        1.34609761e+04, 1.35912628e+04, 1.37228105e+04, 1.38556315e+04,
        1.39897380e+04, 1.41251425e+04, 1.42618576e+04, 1.43998960e+04,
        1.45392703e+04, 1.46799937e+04, 1.48220791e+04, 1.49655397e+04,
        1.51103889e+04, 1.52566400e+04, 1.54043067e+04, 1.55534026e+04,
        1.57039416e+04, 1.58559376e+04, 1.60094048e+04, 1.61643574e+04,
        1.63208097e+04, 1.64787763e+04, 1.66382718e+04, 1.67993111e+04,
        1.69619091e+04, 1.71260808e+04, 1.72918415e+04, 1.74592065e+04,
        1.76281915e+04, 1.77988121e+04, 1.79710840e+04, 1.81450234e+04,
        1.83206463e+04, 1.84979690e+04, 1.86770080e+04, 1.88577799e+04,
        1.90403014e+04, 1.92245896e+04, 1.94106614e+04, 1.95985342e+04,
        1.97882254e+04, 1.99797526e+04, 2.01731336e+04, 2.03683862e+04,
        2.05655287e+04, 2.07645793e+04, 2.09655565e+04, 2.11684789e+04,
        2.13733653e+04, 2.15802348e+04, 2.17891066e+04, 2.20000000e+04
    };

    /**
     * a static IIR filter
     * @tparam FloatType
     */
    template<typename FloatType>
    class Filter {
    public:
        Filter() = default;

        void reset();

        void prepare(const juce::dsp::ProcessSpec &spec);

        void process(juce::AudioBuffer<FloatType> &buffer);

        void setFreq(FloatType x, bool update = true);

        inline FloatType getFreq() const { return static_cast<FloatType>(freq.load()); }

        void setGain(FloatType x, bool update = true);

        inline FloatType getGain() const { return static_cast<FloatType>(gain.load()); }

        void setQ(FloatType x, bool update = true);

        inline FloatType getQ() const { return static_cast<FloatType>(q.load()); }

        void setFilterType(FilterType x, bool update = true);

        inline FilterType getFilterType() const { return filterType.load(); }

        void setOrder(size_t x, bool update = true);

        inline size_t getOrder() const { return order.load(); }

        void addDBs(std::array<FloatType, frequencies.size()> &x, FloatType scale = 1.0);

        void addGains(std::array<FloatType, frequencies.size()> &x, FloatType scale = 1.0);

        inline std::array<FloatType, frequencies.size()> &getDBs() { return dBs; }

        FloatType getDB(FloatType f);

        inline std::array<FloatType, frequencies.size()> &getGains() { return gains; }

        inline juce::uint32 getNumChannels() const { return numChannels.load(); }

        void updateDBs();

        void updateParas();

        size_t getFilterNum() const { return filterNum.load(); }

        std::array<juce::dsp::ProcessorDuplicator<
            juce::dsp::IIR::Filter<FloatType>,
            juce::dsp::IIR::Coefficients<FloatType> >, 16> &getFilters() { return filters; }

        void copyCoeffsFrom(Filter<FloatType> &anotherF);

        juce::ReadWriteLock& getParaLock() {return paraUpdateLock;}

        juce::ReadWriteLock &getMagLock() { return magLock; }

    private:
        std::array<juce::dsp::ProcessorDuplicator<
            juce::dsp::IIR::Filter<FloatType>,
            juce::dsp::IIR::Coefficients<FloatType> >, 16> filters{};
        std::atomic<size_t> filterNum{1};
        std::atomic<double> freq = 1000, gain = 0, q = 0.707;
        std::atomic<size_t> order = 2;
        std::atomic<FilterType> filterType = FilterType::peak;
        juce::dsp::ProcessSpec processSpec{48000, 512, 2};
        juce::ReadWriteLock paraUpdateLock;
        std::atomic<bool> resetFlag;
        std::atomic<juce::uint32> numChannels;

        std::array<FloatType, frequencies.size()> dBs{}, gains{};
        juce::ReadWriteLock magLock;
        std::atomic<bool> magOutdated = false;
    };
}

#endif //ZLEQUALIZER_SINGLE_FILTER_HPP
