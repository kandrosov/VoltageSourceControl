/*!
 * \file ThreadSafeVoltageSource.h
 * \brief Definition of ThreadSafeVoltageSource class.
 * \author Konstantin Androsov (INFN Pisa, Siena University)
 *
 * Copyright 2013, 2014 Konstantin Androsov <konstantin.androsov@gmail.com>
 *
 * This file is part of VoltageSourceControl.
 *
 * VoltageSourceControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * VoltageSourceControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VoltageSourceControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <list>
#include <boost/scoped_ptr.hpp>
#include <mutex>
#include <boost/utility.hpp>

#include "units.h"
#include "IVoltageSource.h"

namespace vsc {
/*!
 * \brief Provides thread-safe access to the voltage source.
 */
class ThreadSafeVoltageSource : public IVoltageSource, private boost::noncopyable {
public:
    /// Measurement collection type.
    typedef std::list<IVoltageSource::Measurement> MeasurementCollection;

    /*!
     * \brief ThreadSafeVoltageSource constructor.
     * \param aVoltageSource - a pointer to the voltage source.
     *
     * To guarantee thread safety \a aVoltageSource should be accessed only through ThreadSafeVoltageSource object. For
     * that reason \a aVoltageSource will be owned by ThreadSafeVoltageSource object and will be destroyed with it.
     */
    explicit ThreadSafeVoltageSource(IVoltageSource* aVoltageSource, bool saveMeasurements = true);

    /// \copydoc IVoltageSource::Set
    virtual Value Set(const Value& value);

    /// \copydoc IVoltageSource::Accuracy
    virtual ElectricPotential Accuracy(const ElectricPotential& voltage);

    /// \copydoc IVoltageSource::Measure
    virtual Measurement Measure();

    /// \copydoc IVoltageSource::Off
    virtual void Off();

    /// Gradually change voltage with given voltage step and delay between steps.
    bool GradualSet(const Value& value, const vsc::ElectricPotential& step, const vsc::Time& delayBetweenSteps,
                    bool checkForCompliance = true);

    /// Returns reference to a collection with all performed measurments.
    /// \remarks ThreadSafeVoltageSource should be locked while accessing the measuremens.
    const MeasurementCollection& Measurements() const { return measurements; }

    /*!
     * \brief Lock the voltage source to be used only in the current thread.
     *
     * The calling thread will be suspended until the lock is obtained. If thread is already own the lock, it will not
     * blocked, however number of Unlock calls should be equal to the number of Lock calls to unlock the object.
     */
    void lock();

    /*!
     * \brief Unlock the voltage source.
     *
     * If number of Unlock calls will be greater than the number of Lock calls, it will lead to the undefined
     * behaviour.
     */
    void unlock();

private:
    std::recursive_mutex mutex;
    boost::scoped_ptr<IVoltageSource> voltageSource;
    bool saveMeasurements;
    MeasurementCollection measurements;
    Value currentValue;
    bool isOn;
};

}
